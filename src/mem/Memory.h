#pragma once

#include <cstdint>
#include <array>
#include <vector>

class Process;

namespace Mem {
    namespace Protect {
        constexpr uint8_t READ = 0x1;
        constexpr uint8_t WRITE = 0x2;
        constexpr uint8_t EXECUTE = 0x4;
    }



    struct Page {
        uintptr_t start;
        size_t size;
        uint8_t protect;
    };



    bool writeData(const Process& proc, uintptr_t address, const void* data, size_t size);

    bool readData(const Process& proc, uintptr_t address, void* out, size_t size);

    template<typename T>
    bool write(const Process& proc, uintptr_t address, const T& value) {
        return writeData(proc, address, static_cast<const void*>(&value), sizeof(T)) > 0;
    }

    template<typename T>
    T read(const Process& proc, uintptr_t address) {
        T ret;
        readData(proc, address, static_cast<void*>(&ret), sizeof(T));
        return ret;
    }

    template<size_t Size>
    bool writeBytes(const Process& proc, uintptr_t address, const std::array<uint8_t, Size>& bytes) {
        return writeData(proc, address, static_cast<const void*>(bytes.data()), bytes.size());
    }

    template<size_t Size>
    std::array<uint8_t, Size> readBytes(const Process& proc, uintptr_t address) {
        std::array<uint8_t, Size> data;
        readData(proc, address, static_cast<void*>(data.data()), data.size());
        return data;
    }

    bool writeBytes(const Process& proc, uintptr_t address, const std::vector<uint8_t>& bytes);

    std::vector<uint8_t> readBytes(const Process& proc, uintptr_t address, size_t size);



    std::vector<Page> enumRwPages(const Process& proc);

    uintptr_t alloc(const Process& proc, size_t size, uint8_t protect = Protect::READ | Protect::WRITE | Protect::EXECUTE);

    void free(const Process& proc, uintptr_t address);



    namespace {
        template <typename Iter1, typename Iter2, typename Iter3>
        bool equalsMask(Iter1 a, Iter2 b, Iter3 mask) {
            for (; *mask; a++, b++, mask++)
                if (*a != *b && *mask == 'x')
                    return false;
            return true;
        }

        template <typename Iter1, typename Iter2>
        bool equals(Iter1 a, Iter2 b, size_t size) {
            for (size_t i = 0; i < size; a++, b++, i++)
                if (*a != *b)
                    return false;
            return true;
        }
    }

    template<size_t Size>
    uintptr_t findSignature(const Process& proc,
                            const std::array<uint8_t, Size>& bytes, const std::string& mask = "",
                            uint32_t align = 1, uint8_t protect = 0) {
        for (auto& page : enumRwPages(proc)) {
            if (page.protect != protect)
                continue;

            std::vector<uint8_t> pageBytes(page.size);
            readData(proc, page.start, static_cast<void*>(pageBytes.data()), page.size);

            bool useMask = mask != "";
            for (int i = 0; i < page.size - bytes.size(); i += align) {
                if (useMask ? equalsMask(pageBytes.data() + i, bytes.data(), mask.data()) :
                    equals(bytes.data(), pageBytes.data() + i, bytes.size()))
                    return page.start + i;
            }
        }
        return 0;
    }
}
