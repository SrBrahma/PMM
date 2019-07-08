#include <stdint.h>
#include <stdlib.h> // For realloc.
#include <circularArray.h>

// Creates the object. If the maxLength is ommited or equal to 0, you will need to call ThEoBjEcT.realloc(length)
// before using the object.
template <class T> CircularArray<T>::CircularArray (int maxLength)
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
    if (!(T*) realloc(mArray, maxLength * sizeof(T)))
        return false;

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
    
    mArray[getLastIndex()] = item;
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
    if (!mCurrentLength) return -1;
    if (item)
        *item = mArray[getLastIndex()];

    mCurrentLength--;

    return 0;
}

// Removes the first item. If given a pointer, the item will be copied to it.
template <class T> int CircularArray<T>::shift(T *item)
{
    if (!mCurrentLength) return -1;
    if (item)
        *item = mArray[mStartIndex];

    mStartIndex++;
    return 0;
}


// Returns the circular array item, relative to the first item.
template <class T> T CircularArray<T>::getItemByFirst(int index)
{
    index = fixIndex(mStartIndex + index);
    if (index < 0) return 0; // Avoids using negative index on the array access.
    return mArray[index];
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




// Returns the index of the last push()ed item.
template <class T> int CircularArray<T>::getLastIndex()
{
    return fixIndex(mStartIndex + mCurrentLength);
}

// Translates negative indexes, and cicles values that are beyond the current array length.
// The index argument is absolute (not relative to the mStartIndex). 
template <class T> int CircularArray<T>::fixIndex(int index)
{
    if (!mMaxLength) return -1;

    index %= mCurrentLength;

    if (index < 0)
        return (  index + mCurrentLength);

    if (index >= mMaxLength)
        return (- index + mCurrentLength);
    
    return index;
}