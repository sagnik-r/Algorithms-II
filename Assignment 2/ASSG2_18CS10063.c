/*Name : Sagnik Roy
 *Roll No: 18CS10063
 *Assignment 2
 *This program uses the inbuilt atan function of math, so use -lm flag while compiling
 *Input is to be accepted from console
 *Output will be displayed to the console*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Point{//DS for point
	double x;
	double y;
};

typedef struct Point point;

struct Segment{//DS for line segment
	point p1;
	point p2;
};

typedef struct Segment segment;

struct Arc{//DS for an arc
	point centre;
	double a1;
	double a2;
};

typedef struct Arc arc;

void merge(point arr[], int l, int m, int r)  
{   //Merge function to sort the points in order of x co-ordinates
    int i, j, k;  
    int n1 = m - l + 1;  
    int n2 = r - m;  
  
    point *L = (point *)malloc(n1*sizeof(point));
    point *R = (point *)malloc(n2*sizeof(point));  
  
    for (i = 0; i < n1; i++)  
        L[i] = arr[l + i];  
    for (j = 0; j < n2; j++)  
        R[j] = arr[m + 1 + j];  
  
    i = 0;   
    j = 0; 
    k = l;  
    while (i < n1 && j < n2) {  
        if (L[i].x < R[j].x) {  
            arr[k] = L[i];  
            i++;  
        }  
        else {  
            arr[k] = R[j];  
            j++;  
        }  
        k++;  
    }  
  
    while (i < n1) {  
        arr[k] = L[i];  
        i++;  
        k++;  
    }  
  
    while (j < n2) {  
        arr[k] = R[j];  
        j++;  
        k++;  
    }  
}  
 
void mergeSort(point arr[], int l, int r)  
{   //Merge sort function to sort the points in order of x coordinates
    if (l < r) {    
        int m = l + (r - l) / 2;  
   
        mergeSort(arr, l, m);  
        mergeSort(arr, m + 1, r);  
  
        merge(arr, l, m, r);  
    }  
}  

int CH(point *S, int n, int flag, point *H);  
int isLeft(point p1, point p2, point p3);
void contzone(point *UH, int u, point *LH, int l, double r, segment *T, arc *A);
void printcontzone(int u, int l, segment *T, arc *A);

int main()
{
	int n;
	scanf("%d", &n);

	double r;
	scanf("%lf", &r);

	point *centre = (point *)malloc(n*sizeof(point));
	for(int i = 0; i<n; i++)
	{
		double a,b;
		scanf("%lf %lf", &a, &b);
		centre[i].x = a;
		centre[i].y = b;
	}

	mergeSort(centre, 0, n-1);//Sort the points

	printf("+++ Circles after sorting\n");//Display the sorted centres
	for(int i=0; i<n; i++)
	{
		printf("    %.15lf %.15lf\n", centre[i].x, centre[i].y);
	}


	point *UH = (point *)malloc(n*sizeof(point));//Upper Hull array
	int u_size = CH(centre, n, 0, UH);//Get Upper Hull

	printf("\n+++ Upper hull\n");//Display Upper Hull
	for(int i=0; i<=u_size; i++)
	{
		printf("    %.15lf %.15lf\n", UH[i].x, UH[i].y);
	}

	point *LH = (point *)malloc(n*sizeof(point));//Lower Hull array 
	int l_size = CH(centre, n, 1, LH);//Get Lower Hull

	printf("\n+++ Lower hull\n");//Display Lower Hull
	for(int i=0; i<=l_size; i++)
	{
		printf("    %.15lf %.15lf\n", LH[i].x, LH[i].y);
	}

	segment *tangent = (segment *)malloc((u_size+l_size)*sizeof(segment));//Tangets of Containment zone
	arc *arcs = (arc *)malloc((u_size+l_size+2)*sizeof(arc));//Arcs of containment Zone

	contzone(UH, u_size, LH, l_size, r, tangent, arcs);//Get containment zone
	printcontzone(u_size, l_size, tangent, arcs);//Print Containment zone
	
}

int CH(point *S, int n, int flag, point *H)
{
	int currIndex = -1;
	if(flag==0)//Flag = 0 ---> Upper Hull (least to highest x coordinate)
	{
		H[currIndex + 1] = S[0];
		currIndex++;

		for(int i=1; i<n; i++)
		{
			if(currIndex == 0)
			{
				H[currIndex + 1] = S[i];
				currIndex++;
			}
			else
			{
				while(currIndex!=0 && isLeft(H[currIndex-1], H[currIndex], S[i]))
				{
					currIndex--;
				}
				H[currIndex + 1] = S[i];
				currIndex++;
			}
		}
	}
	else//Flag = 1 ---> Lower Hull (highest to least x coordinate)
	{
		H[currIndex + 1] = S[n-1];
		currIndex++;

		for(int i=n-2; i>=0; i--)
		{
			if(currIndex == 0)
			{
				H[currIndex + 1] = S[i];
				currIndex++;
			}
			else
			{
				while(currIndex!=0 && isLeft(H[currIndex-1], H[currIndex], S[i]))
				{
					currIndex--;
				}
				H[currIndex + 1] = S[i];
				currIndex++;
			}
		}	
	}
	return currIndex;//CurrIndex = Number of segments in hull
}

int isLeft(point p1, point p2, point p3)//Check if p1, p2 and p3 form a left turn
{
	double val = (p3.y-p2.y)*(p2.x-p1.x) - (p2.y-p1.y)*(p3.x-p2.x);

	return val>0;//Val>0 ---> Left Turn; val<0 ---> Right Turn
}

void contzone(point *UH, int u, point *LH, int l, double r, segment *T, arc *A)
{
	int index = 0;
	for(int i=0; i<=u-1; i++)//Compute the tangents corresponding to upper hull
	{
		double x1 = UH[i].x;
		double y1 = UH[i].y;
		double x2 = UH[i+1].x;
		double y2 = UH[i+1].y;

		double val = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
		double d = sqrt(val);
		double xDel = (r/d)*(y1-y2);
		double yDel = (r/d)*(x2-x1);

		double x3 = x1 + xDel;
		double y3 = y1 + yDel;

		double x4 = x2 + xDel;
		double y4 = y2 + yDel;

		T[index].p1.x = x3;
		T[index].p1.y = y3;

		T[index].p2.x = x4;
		T[index].p2.y = y4;

		index++;
	}
	for(int i = 0; i<=l-1; i++)//Compute the tangents corresponding to lower hull
	{
		double x1 = LH[i].x;
		double y1 = LH[i].y;
		double x2 = LH[i+1].x;
		double y2 = LH[i+1].y;

		double val = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
		double d = sqrt(val);
		double xDel = (r/d)*(y1-y2);
		double yDel = (r/d)*(x2-x1);

		double x3 = x1 + xDel;
		double y3 = y1 + yDel;

		double x4 = x2 + xDel;
		double y4 = y2 + yDel;

		T[index].p1.x = x3;
		T[index].p1.y = y3;

		T[index].p2.x = x4;
		T[index].p2.y = y4;

		index++;
	}

	double pi = 3.141592653589793;// Get the value of pi
	index = 0;

	for(int i = 0; i<=u; i++)// Compute the arcs corresponding to upper hull
	{
		if(i==0)
		{
			point c = UH[i];
			double a1 = pi;
			double slope = (T[0].p1.y-c.y)/(T[0].p1.x-c.x);
			double a2 = atan(slope);
			if(a2<0)
				a2 = a2 + pi;// In Upper Hull all angles are positive

			A[index].centre = c;
			A[index].a1 = a1;
			A[index].a2 = a2;

			index++;
		}
		else if(i==u)
		{
			point c = UH[i];
			double slope = (T[i-1].p2.y-c.y)/(T[i-1].p2.x-c.x);
			double a1 = atan(slope);
			if(a1<0)
				a1 = a1 + pi;// In Upper Hull all angles are positive
			double a2 = 0.0;
			A[index].centre = c;
			A[index].a1 = a1;
			A[index].a2 = a2;

			index++;
		}
		else
		{
			point c = UH[i];
			double slope1 = (T[i-1].p2.y-c.y)/(T[i-1].p2.x-c.x);
			double slope2 = (T[i].p1.y-c.y)/(T[i].p1.x-c.x);
			double a1 = atan(slope1);
			double a2 = atan(slope2);
			if(a1<0)
				a1 = a1 + pi;// In Upper Hull all angles are positive
			if(a2<0)
				a2 = a2 + pi;// In Upper Hull all angles are positive

			A[index].centre = c;
			A[index].a1 = a1;
			A[index].a2 = a2;
			
			index++;
		}
	}
	for(int i = 0; i<=l; i++)// Compute the arcs corresponding to upper hull
	{
		if(i==0)
		{
			point c = LH[i];
			double a1 = 0.0;
			double slope = (T[u].p1.y-c.y)/(T[u].p1.x-c.x);
			double a2 = atan(slope);
			if(a2>0)
				a2 = a2 - pi;// In Upper Hull all angles are negative
			A[index].centre = c;
			A[index].a1 = a1;
			A[index].a2 = a2;

			index++;
		}
		else if(i==l)
		{
			point c = LH[i];
			double a2 = -1.0*pi;
			double slope = (T[u+l-1].p2.y-c.y)/(T[u+l-1].p2.x-c.x);
			double a1 = atan(slope);
			if(a1>0)
				a1 = a1-pi;// In Upper Hull all angles are negative

			A[index].centre = c;
			A[index].a1 = a1;
			A[index].a2 = a2;

			index++;
		}
		else
		{
			point c = LH[i];
			double slope1 = (T[u-1+i].p2.y-c.y)/(T[u-1+i].p2.x-c.x);
			double slope2 = (T[u+i].p1.y-c.y)/(T[u+i].p1.x-c.x);
			double a1 = atan(slope1);
			double a2 = atan(slope2);
			if(a1>0)
				a1 = a1 - pi;// In Upper Hull all angles are negative
			if(a2>0)
				a2 = a2 - pi;// In Upper Hull all angles are negative

			A[index].centre = c;
			A[index].a1 = a1;
			A[index].a2 = a2;

			index++;
		}
	}
}

void printcontzone(int u, int l, segment *T, arc *A)// Print the containment zone
{
	printf("\n+++ The containment zone\n");
	printf("--- Upper section\n");
	for(int i = 0; i<u; i++)
	{
		printf("    Arc     : (%.15lf,%.15lf) From %.15lf to %.15lf\n", A[i].centre.x, A[i].centre.y, A[i].a1, A[i].a2);
		printf("    Tangent : From (%.15lf,%.15lf) to (%.15lf,%.15lf)\n", T[i].p1.x, T[i].p1.y, T[i].p2.x, T[i].p2.y);
	}
	printf("    Arc     : (%.15lf,%.15lf) From %.15lf to %.15lf\n", A[u].centre.x, A[u].centre.y, A[u].a1, A[u].a2);
	printf("--- Lower section\n");
	for(int i = 0; i<l; i++)
	{
		printf("    Arc     : (%.15lf,%.15lf) From %.15lf to %.15lf\n", A[u+1+i].centre.x, A[u+1+i].centre.y, A[u+1+i].a1, A[u+1+i].a2);
		printf("    Tangent : From (%.15lf,%.15lf) to (%.15lf,%.15lf)\n", T[u+i].p1.x, T[u+i].p1.y, T[u+i].p2.x, T[u+i].p2.y);
	}
	printf("    Arc     : (%.15lf,%.15lf) From %.15lf to %.15lf\n", A[u+1+l].centre.x, A[u+1+l].centre.y, A[u+1+l].a1, A[u+1+l].a2);
}