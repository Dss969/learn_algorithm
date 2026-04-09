from sorting.bubble_sort import bubble_sort
from sorting.selection_sort import selection_sort
from sorting.insertion_sort import insertion_sort
from sorting.merge_sort import merge_sort
from sorting.quick_sort import quick_sort
from searching.linear_search import linear_search
from searching.binary_search import binary_search
from data_structures.linked_list import LinkedList
from data_structures.stack import Stack
from data_structures.queue import Queue


def test_bubble_sort():
    assert bubble_sort([64, 34, 25, 12, 22, 11, 90]) == [11, 12, 22, 25, 34, 64, 90]
    assert bubble_sort([]) == []
    assert bubble_sort([1]) == [1]
    assert bubble_sort([3, 1, 2]) == [1, 2, 3]


def test_selection_sort():
    assert selection_sort([64, 25, 12, 22, 11]) == [11, 12, 22, 25, 64]
    assert selection_sort([]) == []
    assert selection_sort([5]) == [5]


def test_insertion_sort():
    assert insertion_sort([12, 11, 13, 5, 6]) == [5, 6, 11, 12, 13]
    assert insertion_sort([]) == []
    assert insertion_sort([1]) == [1]


def test_merge_sort():
    assert merge_sort([38, 27, 43, 3, 9, 82, 10]) == [3, 9, 10, 27, 38, 43, 82]
    assert merge_sort([]) == []
    assert merge_sort([1]) == [1]


def test_quick_sort():
    assert quick_sort([3, 6, 8, 10, 1, 2, 1]) == [1, 1, 2, 3, 6, 8, 10]
    assert quick_sort([]) == []
    assert quick_sort([7]) == [7]


def test_linear_search():
    assert linear_search([2, 3, 4, 10, 40], 10) == 3
    assert linear_search([2, 3, 4, 10, 40], 99) == -1
    assert linear_search([], 1) == -1


def test_binary_search():
    assert binary_search([2, 3, 4, 10, 40], 10) == 3
    assert binary_search([2, 3, 4, 10, 40], 99) == -1
    assert binary_search([], 1) == -1
    assert binary_search([1], 1) == 0


def test_linked_list():
    ll = LinkedList()
    ll.append(1)
    ll.append(2)
    ll.append(3)
    assert ll.to_list() == [1, 2, 3]
    ll.prepend(0)
    assert ll.to_list() == [0, 1, 2, 3]
    ll.delete(2)
    assert ll.to_list() == [0, 1, 3]
    ll.delete(0)
    assert ll.to_list() == [1, 3]


def test_stack():
    s = Stack()
    assert s.is_empty()
    s.push(1)
    s.push(2)
    s.push(3)
    assert s.size() == 3
    assert s.peek() == 3
    assert s.pop() == 3
    assert s.size() == 2


def test_queue():
    q = Queue()
    assert q.is_empty()
    q.enqueue(1)
    q.enqueue(2)
    q.enqueue(3)
    assert q.size() == 3
    assert q.peek() == 1
    assert q.dequeue() == 1
    assert q.size() == 2


if __name__ == "__main__":
    test_bubble_sort()
    test_selection_sort()
    test_insertion_sort()
    test_merge_sort()
    test_quick_sort()
    test_linear_search()
    test_binary_search()
    test_linked_list()
    test_stack()
    test_queue()
    print("All tests passed!")
