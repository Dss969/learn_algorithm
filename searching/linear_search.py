def linear_search(arr, target):
    for i, val in enumerate(arr):
        if val == target:
            return i
    return -1


if __name__ == "__main__":
    data = [2, 3, 4, 10, 40]
    target = 10
    result = linear_search(data, target)
    if result != -1:
        print(f"Element found at index {result}")
    else:
        print("Element not found")
