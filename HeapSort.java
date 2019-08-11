package exercise.exercise1;

import java.util.Arrays;

public class HeapSort extends sort {

	public HeapSort(int[] arr) {
		super(arr);
		// TODO Auto-generated constructor stub
		if(arr.length != 0)
		{
			arrayToSort = new int[arr.length];
			System.arraycopy(arr, 0 , arrayToSort, 0 , arr.length);
		}
	}
	
		public void heapify(int array[], int n , int i)
		{
			int largest = i ;
			int left = 2*i + 1;
			int right = 2*i + 2;
			if(left < n && array[left] > array[largest])
			{
				largest = left;
			}
			if (right < n && array[right] > array[largest]) 
			{
				largest = right;
			}
			if(largest != i)
			{
				//int temp = array[i]; 
	            //array[i] = array[largest]; 
	            //array[largest] = temp; 
				swap(array,i,largest);
	            heapify(array,n,largest);
			}
		}
		public void heapSort(int array[],int n)
		{
			//int n = array.length;
			for(int i=(n/2) - 1; i>=0; i--)
			{
				heapify(array,n,i);
			}
			for(int i = n-1; i>=0; i--)
			{
				//int temp = array[0];
				//array[0] = array[i];
				//array[i] = temp;
				swap(array,0,i);
				heapify(array,i,0);
			}
		}
		
	public void print_result_heap() {
		if(arrayToSort.length != 0)
		{
			heapSort(arrayToSort,arrayToSort.length);
			System.out.println("Array after heap sort");
			System.out.println(Arrays.toString(arrayToSort));
		}
	}
}
