/*  Short job 2
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <algorithm>
#define MAXIMAL 20

struct item{
int x,y,z;
} array_of_cities[MAXIMAL];

float tmp_min;
int no_cities;
float distances[MAXIMAL][MAXIMAL];

//int visited[MAXIMAL];
//int visited2[MAXIMAL][MAXIMAL];


float upBound;
/*

zde muzete pridat nektere globalni datove struktury
you can add some global data structures
*/



float distance(int i,int j)
{
  // Euk. vzd. dvou bodu ve 3D
  // Euc. distnce of 2 points in 3D
  return (sqrt(pow(array_of_cities[i].x-array_of_cities[j].x,2.0)+pow(array_of_cities[i].y-array_of_cities[j].y,2.0)+pow(array_of_cities[i].z-array_of_cities[j].z,2.0)));
}

void countDistances(int no_cities)
{
    #pragma omp parallel for
    for(int i = 0; i < no_cities; i++)
    {
        for(int j = i; j < no_cities; j++)
        {
            distances[i][j] = distance(i,j);
        }
    }

    #pragma omp parallel for
    for(int i = 1; i < no_cities; i++)
    {
        for(int j = 0; j < i; j++)
        {
            distances[i][j] = distances[j][i];
        }
    }
}

void rek(const int actual_city,const int no_visited,const float tmp_dist, const int visited);

void sequence(const int actual_city,const int no_visited,const float tmp_dist, int visited)
{
      int j;
  if (no_visited==no_cities-1)
  {
    float temp3 = tmp_dist + distances[actual_city][0];;
#pragma omp critical
{
      if (temp3<tmp_min)
      {
          tmp_min=temp3;
      }
}
  }
      for(j=1; j < no_cities; j++)
      {
        if ((visited & ( 1 << j )) >> j) continue;
        const float temp = tmp_dist+distances[actual_city][j];
        if(temp < tmp_min)
        {
            visited |= 1 << j;
                sequence(j,no_visited+1,temp, visited);
            visited &= ~(1 << j);
        }
        else j=no_cities;
    }
}

void rek(const int actual_city,const int no_visited,const float tmp_dist, int visited)
{
    for(int j = 1; j < no_cities; j++)
    {
        if ((visited & ( 1 << j )) >> j) continue;
        #pragma omp task
        {
            const float temp = tmp_dist+distances[actual_city][j];
            if(temp < tmp_min)
            {
                int visited2 = visited;
                visited2 |= 1 << j;

                if(no_visited > 2 || no_visited==no_cities-2)
                    sequence(j,no_visited+1,temp, visited2);
                else
                    rek(j,no_visited+1,temp, visited2);
            }
            else j = no_cities;
        }
    }
}



int main(void) {
int i,j,ok;
double t,p,start,end;
int   capacity,d,sum;
float correct[MAXIMAL+1]={0,0,0,0,0,0,579.8,807.135,865.336,911.066,1017.8,1155.75,1247.51,1186.36,1285.9,1334.59,1385.01,1408.21,1539.41,1580.84,1601.22};
  no_cities=5;

  start=omp_get_wtime();
  do{
      no_cities++;
      // reinicializace vstupu
      // reinicialization of input data
      sum=0.0;
      for(j=0;j<no_cities;j++)
      {
        i=j+no_cities;
        array_of_cities[j].x=109+37*(i%5)+23*(i%7)+5*(i%31)+(i%3);
        array_of_cities[j].y=171+19*(i%13)+7*(i%17)+5*(i%19)+(i%3);
        array_of_cities[j].z=117+9*(i%13)+17*(i%7)+5*(i%23)+(i%11);
        //visited[j]=0;
      }
      // reset computation !!!!
      tmp_min=0;//1e+20;      // or something like that
      /*
        you can add some precomputation here
        zde mozno pridat preproccesing
      */

      countDistances(no_cities);
      for(int i = 0; i < no_cities-1; i++)
      {
        tmp_min+=distances[i][i+1];
      }
      tmp_min+=distances[no_cities-1][0];

/*-------------------------------------------------*/
      // main (parallel) computation

#pragma omp parallel num_threads(2000)
#pragma omp single
{
      rek(0,0,0.0, 0);
}

      printf("cities=%i min. TSP =%g\n",no_cities,tmp_min);
      fflush(stdout);

      if(fabs(tmp_min-correct[no_cities])>0.05)
      {
        printf("ERROR!!!!!\n");
        break;
      }

      ok=1;
      if (no_cities>=MAXIMAL) ok=0;
      end=omp_get_wtime();
      if ((end-start)>=5.0) ok=0;
  }while(ok);

  printf("\n");

  printf("end=%i\n",no_cities);
  t=end-start;
  printf("time=%g\n",t);
  p=3.0*(no_cities-12)+0.5*29.2/t-0.5;
  if (p>15.0) p=15.0;
  printf("points=%g\n",p);
  return 0;
}

