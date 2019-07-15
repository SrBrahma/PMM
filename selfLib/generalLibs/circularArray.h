// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma)

#ifndef CIRCULAR_ARRAY_h
#define CIRCULAR_ARRAY_h

#include <Arduino.h> // For realloc.
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
    int  getLastIndex();
    int  getCurrentIndex();

    // Fixes the given index.
    int  fixIndex(int index);

    T    *mArray        = NULL;
    int  mMaxLength     = 0;
    int  mStartIndex    = 0;
    int  mCurrentLength = 0;
};



// Creates the object. If the maxLength is ommited or equal to 0, you will need to call ThEoBjEcT.realloc(length)
// before using the object.
template <class T> CircularArray<T>::CircularArray(int maxLength)
{
    if (maxLength == 0)
        reset();
    else
        realloc(maxLength);
}
template <class T> CircularArray<T>::~CircularArray()
{
    free(mArray);
}

// Resets the array. But won't free() the already allocated array.
template <class T> void CircularArray<T>::reset()
{
    mStartIndex = mCurrentLength = 0;
}

// Changes the array length. Returns true if successful, false otherwise.
// Reset is called if the realloc is successful, as I won't need rearraging right now.
template <class T> bool CircularArray<T>::realloc(int maxLength)
{
    T *newArray = (T*) ::realloc(mArray, maxLength * sizeof(T));
    if (!newArray)
        return false;

    mArray = newArray;

    mMaxLength = maxLength;
    reset();
    return true;
}





// Returns how many positions are available. 0 means is full.
template <class T> int CircularArray<T>::available() { return mMaxLength - mCurrentLength; }
// Returns the current used length.
template <class T> int CircularArray<T>::length()    { return mCurrentLength; }
// Returns the total usable length.
template <class T> int CircularArray<T>::maxLength() { return mMaxLength; }


// Adds an item to the end, if there is available space.
template <class T> int CircularArray<T>::push(T item)
{
    if (!mMaxLength) return -1;
    if (mCurrentLength >= mMaxLength) return -2;

    mArray[getCurrentIndex()] = item;
    mCurrentLength++;

    return 0;
}

// Adds an item to the end even if the circular array is full, in this case, removing the first item in the array (shift()).
template <class T> int CircularArray<T>::forcePush(T item)
{
    if (!mMaxLength) return -1;
    if (mCurrentLength == mMaxLength)
    {
        shift();
        return 1;
    }
    return push(item);
}

// Removes the last item. If given a pointer, the item will be copied to it.
template <class T> int CircularArray<T>::pop(T *item)
{
    if (!mMaxLength)     return -1;
    if (!mCurrentLength) return -2;

    if (item)
        *item = mArray[getLastIndex()];

    mCurrentLength--;

    return 0;
}

// Removes the first item. If given a pointer, the item will be copied to it.
template <class T> int CircularArray<T>::shift(T *item)
{
    if (!mMaxLength)     return -1;
    if (!mCurrentLength) return -2;

    if (item)
        *item = mArray[mStartIndex];

    mStartIndex = (mStartIndex + 1) % mMaxLength; // increase and cycle if needed.
    mCurrentLength--;
    return 0;
}


// Returns the circular array item, relative to the first item.
template <class T> T CircularArray<T>::getItemByFirst(int index)
{
    return mArray[fixIndex(index)];
}
// Returns by the second arg, the circular array item, relative to the first item.
template <class T> int CircularArray<T>::getItemByFirst(int index, T *returnItem)
{
    if (!returnItem)     return -1;
    if (!mMaxLength)     return -2;
    if (!mCurrentLength) return -3;
    *returnItem = getItemByFirst(index);
    return 0;
}

// Returns the item of the circular array, relative to the last item.
template <class T> T CircularArray<T>::getItemByLast(int index)
{
    return getItemByFirst(index - 1);
}
// Returns by the second arg, the circular array item, relative to the last item.
template <class T> int CircularArray<T>::getItemByLast(int index, T *returnItem)
{
    return getItemByFirst(index - 1, returnItem);
}


// Returns the index of the current index, where the next push() item will enter.
template <class T> int CircularArray<T>::getCurrentIndex()
{
    if (!mMaxLength) return -1;

    return (mStartIndex + mCurrentLength) % mMaxLength;
}
// Returns the index of the current index, where the last push() item has entered.
template <class T> int CircularArray<T>::getLastIndex()
{
    if (!mMaxLength)     return -1;
    if (!mCurrentLength) return -2;

    return (mStartIndex + mCurrentLength - 1) % mMaxLength;
}

// Translates negative indexes, and cicles values that are beyond the current array length.
// The index argument is absolute (not relative to the mStartIndex). 
template <class T> int CircularArray<T>::fixIndex(int index)
{
    if (!mMaxLength)     return -1;
    if (!mCurrentLength) return -2;

    index = (index % mCurrentLength) + mStartIndex;

    // Consider the second argument being the mStartIndex.
    // ex [4,5,6,7,8,9]. fixIndex(-2, 0) should retrieve 8 (index 4). We just need to sum the length with the negative index,
    // 6 + (-2) = 4. arr[4] = 8.
    // ex [,,4,5,6,]. fixIndex(-1, 2), which firstly produces index = 1, should retrieve 6 (index 4). Again, sum length with index.
    // 3 + 1 (result of (-1 % 3) + 2) = 4;
    // ex [8,9,,,4,5,6,7]. fixIndex(-2, 4), should retrieve 8 (index 0). Again, sum length with index.
    // 6 + 2 (result of (-2 % 6) + 4) = 8. Will be % mMaxLength below, which produces 0.
    if (index < mStartIndex)
        index += mCurrentLength;
    
    if (index >= mMaxLength)
        index %= mMaxLength;

    return index;
}

#endif