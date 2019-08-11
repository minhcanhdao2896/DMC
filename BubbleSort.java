package exercise.exercise1;
import java.util.Arrays;
public class BubbleSort extends sort {
     public BubbleSort(int[] arrays) {
		super(arrays);
		if(arrays.length != 0)
		{
			arrayToSort = new int[arrays.length];
			System.arraycopy(arrays, 0 , arrayToSort, 0 , arrays.length);
		}
	}
     protected void Buble_doSort(int arr[]) {
 		int n=arr.length;
 		int temp = 0; 
 		for(int i=0;i<n;i++)
 		{
 			for(int j=1; j<(n-i); j++) 
 			{
 				if(arr[j-1] > arr[j])
 				{
 					temp=arr[j-1];
 					arr[j-1]=arr[j];
 					arr [j]=temp;
 					//swap(array,j,j-1);
 				}
 			}
 		}
 	}
 	public void print_result_bubble() {
 		if(arrayToSort.length != 0)
 		{
 			Buble_doSort(arrayToSort);
 			System.out.println("Array after bubble sort");
 			System.out.println(Arrays.toString(arrayToSort));
 		}
 	}
 }




