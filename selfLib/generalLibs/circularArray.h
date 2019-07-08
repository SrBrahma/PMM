// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma)

#ifndef CIRCULAR_ARRAY_h
#define CIRCULAR_ARRAY_h


#include <stdint.h> // for uint16_t

// This must always allow indexing out of range, using getItemByXYZ. Ex: "a = [5,6,7]. a[0] is 5, a[3] is 5, a[-3] is 5".
template <class T>
class CircularArray
{
public:
    // Creates the object. If the maxLength is ommited or equal to 0, you will need to call ThEoBjEcT.realloc(length)
    // before using the object.
    CircularArray(int maxLength = 0);
    // To free the array.
    ~CircularArray();
    // Resets the array.
    void reset();
    // Changes the array length. Returns true if successful, false otherwise.
    // Reset is called if the realloc is successful, as I won't need rearraging right now.
    bool realloc(int maxLength);


    // Returns the item of the circular array, relative to the first item.
    // Ex: Array is [9,,,6,7,8]. First item is 6, last is 9. (gI = getItem)
    // gI(0) returns 6, gI(2) returns 8, gI(3) returns 9. gI(4) returns 6 again (it cycles!).
    // gI(-1) retuns 9, gI(-2) returns 8.
    // Only use this if you are sure that it won't have any problem. Else, use the function
    // that returns the item by the second argument, and the error code as normal return.
    T    getItemByFirst(int index);
    // Returns by the second argument the item of the circular array, relative to the first item.
    // Ex: Array is [9,,,6,7,8]. First item is 6, last is 9. (gI = getItem)
    // gI(0) returns (by the 2nd arg) 6, gI(2) returns 8, gI(3) returns 9. gI(4) returns 6 again (it cycles!).
    // gI(-1) retuns 9, gI(-2) returns 8.
    // The real return, returns 0 if success. Negative if error.
    int  getItemByFirst(int index, T *returnItem);

    // Returns the item of the circular array, relative to the last item.
    // This functions is the same as the getItemByFirst(), but the index is substracted by 1.
    // Ex: Array is [9,,,6,7,8]. Last item is 9, first is 6.
    // gI(0) returns 9, gI(-1) returns 8, gI(-3) returns 6. gI(-4) returns 9 again (it cycles!).
    // gI(1) retuns 6, gI(2) returns 8.
    // Only use this if you are sure that it won't have any problem. Else, use the function
    // that returns the item by the second argument, and the error code as normal return.
    T    getItemByLast(int index);
    // Returns by the second argument the item of the circular, relative to the last item.
    // This functions is the same as the getItemByFirst(), but the index is substracted by 1.
    // Ex: Array is [9,,,6,7,8]. Last item is 9, first is 6.
    // gI(0) returns (by the 2nd arg) 9, gI(-1) returns 8, gI(-3) returns 6. gI(-4) returns 9 again (it cycles!).
    // gI(1) retuns 6, gI(2) returns 8.
    // The real return, returns 0 if success. Negative if error.
    int  getItemByLast(int index, T *returnItem);

    // Returns how many positions are available.
    int  available();
    // Returns the current used length.
    int  length();
    // Returns the total usable length.
    int  maxLength();

    // Adds an item to the end. Won't add if array is full.
    // Return 0 if pushed successfully. Negative if error.
    int  push(T item);

    // Adds an item to the end even if the circular array is full, in this case, removing the first item in the array (shift()).
    // Returns 0 if pushed without shifting. 1 if shifted. Negative if error.
    int  forcePush(T item);

    // Removes the last item. If given a pointer, the item will be copied to it.
    // Returns 0 if popped successfully. Negative if error.
    int  pop(T *item = NULL);

    // Removes the first item. If given a pointer, the item will be copied to it.
    // Returns 0 if shifted successfully, Negative if error.
    int  shift(T *item = NULL);


private:

    int getLastIndex();

    // Fixes the given index.
    int fixIndex(int index);

    T   mArray[]       = NULL;
    int mMaxLength     = 0;
    int mStartIndex    = 0;
    int mCurrentLength = 0;
};

#endif