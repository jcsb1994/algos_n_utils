/*! Created on Sun Mar 05 2023
    \author jcsb1994
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    \brief Tools and algorithms to work with arrays
    
    Does not use qsort, sort, nor any std algo
*/

namespace  array_tools
{
/*! \brief  Get the idx of largest element in value */
template <class T>
const unsigned int  get_largest_elem_idx(T* buffer, const unsigned int size);
template <class T>
const unsigned int  get_smallest_elem_idx(T* buffer, const unsigned int size);

/*! \brief Get the idx of nth largest element
    \note e.g. pos 0 returns buffer's largest element, 1 returns second largest
    \return -1 if pos is invalid */
template <class T>
const int           get_nth_largest_elem_idx(T* buffer, const unsigned int size, const unsigned int nth);
template <class T>
const unsigned int  get_nth_largest_elem_idx_climbing(T* buffer, const unsigned int size, const unsigned int pos);
template <class T>
const unsigned int  get_nth_largest_elem_idx_descending(T* buffer, const unsigned int size, const unsigned int pos);


// Definitions
template <class T>
const int get_nth_largest_elem_idx(T* buffer, const unsigned int size, const unsigned int nth)
{
    if(nth >= size) {return -1; }

    if (nth < (size / 2)) {
        return get_nth_largest_elem_idx_climbing<T>(buffer, size, nth);
    } else {
        return get_nth_largest_elem_idx_descending<T>(buffer, size, nth);
    }
}

template <class T>
const unsigned int get_largest_elem_idx(T* buffer, const unsigned int size)
{
    unsigned int largest_elem_idx = 0;
    for (unsigned int i = 0; i < size; i++) {
        if (buffer[largest_elem_idx] < buffer[i]) {
            largest_elem_idx = i;
        }
    }
    return largest_elem_idx;
}

template <class T>
const unsigned int get_smallest_elem_idx(T* buffer, const unsigned int size)
{
    unsigned int smallest_elem_idx = size-1;
    for (unsigned int i = 0; i < size; i++) {
        if (buffer[smallest_elem_idx] > buffer[i]) {
            smallest_elem_idx = i;
        }
    }
    return smallest_elem_idx;
}

template <class T>
const unsigned int get_nth_largest_elem_idx_climbing(T* buffer, const unsigned int size, const unsigned int pos)
{
    unsigned int ret = get_largest_elem_idx(buffer, size);
    if(pos == 0) { return ret; } // return largest (0th position)
    
    unsigned int lastIdx = ret;
    for (unsigned int i = 1; i <= pos; i++) { // Already have 1 out
        T closestDiff = (1 << 8*sizeof(T)) - 1; // Set type data to all 1's
        unsigned int equalCount = 0; // count equal values
        for (unsigned int j = 0; j < size; j++) {
            // Smaller than last data and diff is smaller (looking for very next largest)
            if (buffer[lastIdx] > buffer[j]) {
                T diff = buffer[lastIdx] - buffer[j];
                if (diff < closestDiff) {
                    closestDiff = diff;
                    ret = j;
                }
            // Treat equal elements
            } else if ((buffer[j] == buffer[lastIdx]) && (j != lastIdx)) {
                equalCount++;
                if ((equalCount + i) > pos) {
                    return j;
                }
            }
        }
        i += equalCount;
        lastIdx = ret;
    }
    return ret;  
}

template <class T>
const unsigned int get_nth_largest_elem_idx_descending(T* buffer, const unsigned int size, const unsigned int pos)
{
    unsigned int posIdx = 0;
    unsigned int lastIdx;

    for (int iPos = pos; iPos >= 0; iPos--) {
        if (iPos == pos) {
            for (int i = size - 1; i >= 0; i--) {
                if (buffer[posIdx] < buffer[i]) {
                    posIdx = i;
                }
            }
    } else {
        T closestDiff = (1 << 8*sizeof(T)) - 1; // Set type data to all 1's
        unsigned int equalCount = 0;
        for (int i = size - 1; i >= 0; i--) {
        if ((buffer[i] == buffer[lastIdx]) && (i != lastIdx)) {
            equalCount++;
            if ((equalCount + iPos) > pos) {
            return i;
            }
        } else if ((buffer[lastIdx] > buffer[i]) &&
                    ((buffer[lastIdx] - buffer[i]) < closestDiff)) {
            closestDiff = buffer[lastIdx] - buffer[i];
            posIdx = i;
        }
        }
        iPos -= equalCount;
    }
    lastIdx = posIdx;
    }
    return posIdx;

}


// template <class T>
// const unsigned int get_nth_largest_elem_idx_descending(T* buffer, const unsigned int size, const unsigned int pos)
// {
//     unsigned int ret = get_largest_elem_idx(buffer, size);
//     if(pos == 0) { return ret; } // return largest (0th position)
    
//     unsigned int lastIdx = ret;
//     for (unsigned int i = pos; i > 0; i--) {
//         T closestDiff = (1 << 8*sizeof(T)) - 1; // Set type data to all 1's
//         unsigned int equalCount = 0; // count equal values

//         for (unsigned int j = size-1; j > 0; j--) { // DIFF 1
//             // Smaller than last data and diff is smaller (looking for very next largest)
//             if (buffer[lastIdx] > buffer[j]) {
//                 T diff = buffer[lastIdx] - buffer[j];
//                 if (diff < closestDiff) {
//                     closestDiff = diff;
//                     ret = j;
//                 }
//             // Treat equal elements
//             } else if ((buffer[j] == buffer[lastIdx]) && (j != lastIdx)) {
//                 equalCount++;
//                 if ((equalCount + i) > pos) {
//                     return j;
//                 }
//             }
//         }
//         i -= equalCount; // DIFF 2
//         lastIdx = ret;
//     }
//     return ret;  
// }
// 

// template <class T>
// const unsigned int get_nth_largest_elem_idx_descending(T* buffer, const unsigned int size, const unsigned int pos)
// {
//     unsigned int ret = get_largest_elem_idx(buffer, size);
//     if(pos == 0) { return ret; } // return largest (0th position)
    
//     unsigned int lastIdx = ret;
//     for (unsigned int i = pos; i > 1; i--) {
//         T closestDiff = (1 << 8*sizeof(T)) - 1; // Set type data to all 1's
//         unsigned int equalCount = 0; // count equal values

//         for (unsigned int j = size; j > 0; j--) { // DIFF 1
//             // Smaller than last data and diff is smaller (looking for very next largest)
//             if (buffer[lastIdx] > buffer[j]) {
//                 T diff = buffer[lastIdx] - buffer[j];
//                 if (diff < closestDiff) {
//                     closestDiff = diff;
//                     ret = j;
//                 }
//             // Treat equal elements
//             } else if ((buffer[j] == buffer[lastIdx]) && (j != lastIdx)) {
//                 equalCount++;
//                 if ((equalCount + i) > pos) {
//                     return j;
//                 }
//             }
//         }
//         i -= equalCount; // DIFF 2
//         lastIdx = ret;
//     }
//     return ret;  
// }


}