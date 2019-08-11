package exercise.exercise1;
import java.util.Arrays;
public class MergeSort extends sort {
	
public MergeSort(int[] arr) {
		super(arr);
		// TODO Auto-generated constructor stub
		if(arr.length != 0)
		{
			arrayToSort = new int[arr.length];
			System.arraycopy(arr, 0 , arrayToSort, 0 , arr.length);
		}
	}


	public void merger(int arr[], int l, int m, int r)
	{
		int n1 = m - l + 1;
		int n2 = r - m;
		 /* Create temp arrays */
        int L[] = new int [n1]; 
        int R[] = new int [n2]; 
        /*Copy data to temp arrays*/
        for (int i=0; i<n1; i++) 
        {
            L[i] = arr[l + i];
        }
        for (int j=0; j<n2; j++) 
        {
            R[j] = arr[m + 1+ j];
        }
        /* Merge the temp arrays */
        
        // Initial indexes of first and second subarrays 
        int i = 0, j = 0; 
  
        // Initial index of merged subarry array 
        int k = l; 
        while (i < n1 && j < n2) 
        { 
            if (L[i] <= R[j]) 
            { 
            	arr[k] = L[i]; 
                i++; 
            } 
            else
            { 
            	arr[k] = R[j]; 
                j++; 
            } 
            k++; 
        } 
        /* Copy remaining elements of L[] if any */
        while (i < n1) 
        { 
        	arr[k] = L[i]; 
            i++; 
            k++; 
        } 
  
        /* Copy remaining elements of R[] if any */
        while (j < n2) 
        { 
        	arr[k] = R[j]; 
            j++; 
            k++; 
        } 
	}
	public void mergerSort(int arr[],int l, int r)
	{
		if (l < r) 
        { 
            // Find the middle point 
            int m = (l+r)/2; 
  
            // Sort first and second halves 
            mergerSort(arr, l, m); 
            mergerSort(arr , m+1, r); 
  
            // Merge the sorted halves 
            merger(arr, l, m, r); 
        } 
	}
	
public void print_result_merge() {
		if(arrayToSort.length != 0)
		{
			mergerSort(arrayToSort,0,arrayToSort.length-1);
			System.out.println("Array after merge sort");
			System.out.println(Arrays.toString(arrayToSort));
		}
	}
}



