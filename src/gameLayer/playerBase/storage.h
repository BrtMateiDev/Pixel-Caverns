#ifndef MYGAME_STORAGE_H
#define MYGAME_STORAGE_H

#pragma once
#include <unordered_map>

inline struct Storage {
    std::unordered_map<unsigned short int, unsigned int> storedOres;

    void depositOres(std::unordered_map<unsigned short int, unsigned int> &minedOres) {
        for (const auto &[type, amount]: minedOres) {
            storedOres[type] += amount;
        }
        minedOres.clear();
    }
} storage;

#endif //MYGAME_STORAGE_H
