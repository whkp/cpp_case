# Leetcode

## 子串

### 和为k的子数组

<img src="E:\cpp\Leetcode\pic\560.png" alt="560" style="zoom:50%;" />

**思路：前缀和+哈希表**

前缀和： 用pre[i]表示nums中0到i的值，和为k的子数组就是pre[i] - pre[j-1]（j到i的数组）

哈希表：存放前缀和和出现的次数（nums中包含负数）

循环一次即可解决

**易错**：用哈希表应初始化一个（0，1）的键值对

### [滑动窗口最大值](https://leetcode.cn/problems/sliding-window-maximum/)

<img src="E:\cpp\Leetcode\pic\239.png" alt="239" style="zoom:50%;" />

**思路**：滑动窗口内构造一个单调递减的双端队列（deque）；

<img src="E:\cpp\Leetcode\pic\239_answer.png" alt="239_answer" style="zoom:50%;" />

**易错:**在循环中形成滑动窗口后应将第一次的最大值放进结果中；



## 二分查找

**二分查找示意图：**

![erfen](E:\cpp\Leetcode\pic\erfen.gif)

****

***tips：在一个单调或者局部单调有序数组中查找一个符合某些条件的值，时间复杂度为O(logN)***



### 35 [搜索插入位置](https://leetcode.cn/problems/search-insert-position/)

<img src="E:\cpp\Leetcode\pic\35.png" alt="35" style="zoom:50%;" />

题目让找到**第一个大于等于target值的**下标，如果所有树都小于target，返回数组长度。

可以采用闭区间做法（左右两个边界可以取到），循环过程中左右两个边界值都要在中间位置再多移动一次

```c++
    int searchInsert(vector<int>& nums, int target) {
        int left = 0, right = nums.size() - 1;
        //所有数都大于target，right会收缩到left = 0
        //所有数都小于target，只会移动left，最后会加到right+1，即数组长度
        while (left <= right) {
            //闭区间做法，可以取到左右两个位置
            //取等号时移动右边让其收缩到左边的位置
            if (target > nums[left + (right - left) / 2]) {
                left = left + (right - left) / 2 + 1;
            } else {
                right = left + (right - left) / 2 - 1;
            }
        }
        return left;
    }

```

