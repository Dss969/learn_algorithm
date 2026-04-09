from typing import List

class Solution:
    def calcute_twosum(self,numbers: List[int],target: int) -> List[int]:
        left =0
        right = len(numbers) - 1

        while left < right:
            s = numbers[left] + numbers[right]

            if s == target:
                break
        
            if s>target:

                right -= 1
            else:
                
                left += 1

        return [left+1,right+1]