//Pentru bonus: problema care aloca X blocuri de memorie citite de la tastatura si le initializeaza cu 0


#include <iostream>
#include <cstdint>
#include <Windows.h>


int main() {
    int x;

    std::cout << "Introduceti numarul de blocuri de memorie: ";
    std::cin >> x;

    SIZE_T size = x * sizeof(uint8_t);

    LPVOID memoryBlock = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (memoryBlock == NULL) {
        std::cerr << "Eroare la alocarea memoriei!" << std::endl;
        return 1;
    }

    ZeroMemory(memoryBlock, size);

    uint8_t* memory = static_cast<uint8_t*>(memoryBlock);

    std::cout << "Blocurile de memorie au fost initializate la 0: ";
    for (int i = 0; i < x; ++i) {
        std::cout << (int)memory[i] << " ";
    }
    std::cout << std::endl;

    if (VirtualFree(memoryBlock, 0, MEM_RELEASE) == 0) {
        std::cerr << "Eroare la eliberarea memoriei!" << std::endl;
        return 1;
    }

    return 0;
}