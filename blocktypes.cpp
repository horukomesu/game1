
#include "blocktypes.h"

// ����� ������ ��� ������ ����� ������ (���� ���)
std::vector<BlockType> blockTypes;

// ���������� ������� ������������� (������ ���� ���� ������!)
void InitBlockTypes() {
    blockTypes.clear();

    blockTypes.push_back({ 0, {0,0,0,0,0,0} });    // air
    blockTypes.push_back({ 1, {0,2,2,2,2,2} });    // dirt 
    blockTypes.push_back({ 2, {2,0,1,1,1,1} });    // grass 
    blockTypes.push_back({ 3, {3,3,3,3,3,3} });    // stone 
    blockTypes.push_back({ 4, {4,4,4,4,4,4} });    // water
    blockTypes.push_back({ 5, {5,5,5,5,5,5} });    // sand
    blockTypes.push_back({ 6, {6,6,6,6,6,6} });    // clay
    blockTypes.push_back({ 7, {7,7,7,7,7,7} });    // gravel
    blockTypes.push_back({ 8, {8,8,8,8,8,8} });    // ice
    blockTypes.push_back({ 9, {9,9,9,9,9,9} });    // mud
    blockTypes.push_back({ 10, {3,3,3,3,3,3} });    // ???
}
