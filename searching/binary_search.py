def binary_search(arr, target):
    low, high = 0, len(arr) - 1
    while low <= high:
        mid = (low + high) // 2
        if arr[mid] == target:
            return mid
        elif arr[mid] < target:
            low = mid + 1
        else:
            high = mid - 1
    return -1


if __name__ == "__main__":
    data = [2, 3, 4, 10, 40]
    target = 10
    result = binary_search(data, target)
    if result != -1:
        print(f"Element found at index {result}")
    else:
        print("Element not found")
