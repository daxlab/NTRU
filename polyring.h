/*--------------------------------------------------------------------------------------
Header file including helper functions to carry out polynomial operations required for
NTRUEncrypt
All poly_ functions don't return any value but store the output in the first parameter
i.e. They work like a+=b, a*=b etc.
Use dbgpoly(x) and dbgint(x) for debugging
----------------------------------------------------------------------------------------*/



#ifndef POLYRING_H
#define POLYRING_H

#define maxx 10000
#define dbgint(x) printf(#x": %d\n",x)
#define dbgpoly(poly) printf(#poly"(X): ");poly_display(poly, n+1)
#define swp(x,y) x = x + y ; y = x - y; x = x - y;
//Prototypes of all functions

//void poly_init(int poly[], int n);
//void poly_assign(int poly1[], int poly2[], int n);
//void poly_add(int poly1[], int poly2[], int n);
//void poly_swap(int poly1[], int poly2[], int n);
//void poly_mod(int poly[], int n, int mod);
//void poly_mul_poly(int poly1[], int poly2[] , int n , int mod);
//void poly_display(int poly[], int n);
//void poly_mul_int(int poly[], int k, int n);
//void poly_mul_X(int poly[], int n);
//void poly_div_X(int poly[], int n);
//void poly_shift(int poly[] , int k, int n);
//void poly_ring_shift( int poly[] , int p , int n);
//void poly_inverse_primepower( int poly[] , int invpoly[], int p , int r , int n);
//void poly_shuffle( int poly[] , int n);
//int deg(int poly[], int n);
//int modinv(int r,int r0);
//int swap(int *x, int *y);

int i;

//Definitions start from here******************************************************************************************
//Polynomial initialisation
void poly_init(int poly[], int n)
{
    for( i = 0; i < n; i++)poly[i]=0;

}


// copying one polynomial to another
void poly_assign(int poly1[], int poly2[], int n)
{
    int i;
    for(i = 0;i < n; i++)poly1[i]=poly2[i];
}

//addition
void poly_add(int poly1[], int poly2[], int n)
{

    for ( i = 0 ; i < n ; i++)poly1[i]+=poly2[i];
}
//swapping
void poly_swap(int poly1[], int poly2[], int n)
{
    int temp;
    for ( i = 0 ; i < n ; i++)
    {
        temp=poly1[i];
        poly1[i]=poly2[i];
        poly2[i]=temp;

    }
}

// cyclic shifting coefficients k times
void poly_shift(int poly[], int k , int n)
{
    int poly2[maxx] ;
    poly_init(poly2 , n);

    for(i = 0 ; i < n ; i++)
    {
        poly2[i] = poly[(i+k)%n];
    }

    poly_swap(poly , poly2 , n);
}


//multiplying polynomial by constant
void poly_mul_int(int poly[], int k, int n)
{

    for ( i = 0 ; i < n ; i++) poly[i]*=k;

}

// multiplying by X or shifting right by 1
void poly_mul_X(int poly[], int n)
{

    for ( i = n-1; i > 0 ; i--) poly[i] = poly[i-1];
    poly[0] = poly[n];
}

//division by X or shifting left by 1
void poly_div_X(int poly[], int n)
{

    for ( i = 0; i < n ; i++) poly[i] = poly[i+1];

}

// display polynomial
void poly_display( int poly[], int n)
{
    for( i = 0; i < n; i ++ )
       if(poly[i]) printf("%dX^%d + ",poly[i],i);

    printf("\n");
}

//degree of polynomial .. for zero polynomial deg is -1
int deg(int poly[] , int n)
{
    int d=-1;
    for( i = 0; i < n ; i++)
    {
        if(poly[i] != 0) d = i;
    }
    return d ;
}

//modular inverse of an integer r modulo r0
int modinv(int r,int r0)
{
    int l,j,q,m=r0,s=1,s0=0;
    while(r)
    {
        l=r;j=s;
        q=r0/r;
        r=r0-q*r;
        s=s0-q*s;
        r0=l;s0=j;
    }
    s0%=m;
    if( s0<0)  return s0+m;
    return s0;
}

//polynomial modulo integer in finite field (0..... mod -1)
void poly_mod(int poly[], int n, int mod)
{
    for( i = 0 ; i < n ; i++ )
    {
        poly[i] %= mod;
        if( poly[i] < 0 ) poly[i] += mod;

    }
}

// multiplication of 2 polynomials ( O(n^2))
void poly_mul_poly(int poly1[], int poly2[] , int n , int mod)
{
    int j , finalcoef;
    int finalp[maxx];
    poly_init(finalp , n+1);
    for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			finalp[(i+j)%n]+=(poly1[i]*poly2[j]);
		}
	}

	for(i=0;i<n;i++)
    {
        finalcoef=finalp[i]%mod;
        if(finalcoef<0) finalcoef+=mod;
        poly1[i]=finalcoef;
    }
}

//Shifting finite field from (0....p-1) to (-p/2.... p/2)
void poly_ring_shift( int poly[] , int p , int n)
{
    int finalcoef;
	for(i = 0 ; i < n ;i++)
    {
        finalcoef = poly[i]%p;
        if(finalcoef > p/2) finalcoef-=p;
        else if(finalcoef < -p/2) finalcoef+=p;
        poly[i] = finalcoef;
    }
}

/*-------------------------------------------------------------
 Polynomial Inverse  of poly1(X) in the Field (Z/pZ)[X]/poly2(X)

 Here poly1 is the input function of the form a0x^0 + a1x^1.... an-1 x^ n-1
 To generate key poly2 MUST be X^n - 1
 p must be a prime number
 Answer is stored in poly1(X)
---------------------------------------------------------------*/


void  poly_inverse( int poly[], int poly2[], int n, int p)
{

    //freopen("input.txt","r",stdin);
    //freopen("output.txt","w",stdout);

    //All calculations are done in the field (0... p-1) .
    //To shift field use poly_ring_shift for ( -p/2... p/2 )
    int b[maxx], c[maxx], f[maxx], g[maxx];
    int gtemp[maxx] , ctemp[maxx];
    int k,  u , j, coeff,  dg;

    poly_init(b, n+1);
    poly_init(c, n+1);
    poly_init(f, n+1);
    poly_init(g, n+1);


//Initilisations
    k = 0;
    b[0] = 1;
    c[0] = 0;
    poly_assign(f, poly, n+1);
    //g[n] = 1;  g[0] = -1;// X ^ N - 1

    poly_assign( g , poly2 , n+1);
    poly_mod(f , n+1 , p);
    poly_mod(g , n+1 , p);


//Algorithm starts here
    while(1)
    {

        while ( f[0] == 0 )
        {
            poly_div_X( f, n+1);
            poly_mul_X( c, n+1);
//            printf("Ater mul_X..\n");
//            dbgpoly(f);
//            getchar();

            k++;
        }


        if( deg ( f, n+1) == 0)
        {
            int f1= modinv(f[0] , p);
            poly_mul_int(b , f1 , n+1);
            poly_shift( b , k , n );

            break;
        }


        if( deg( f , n+1 ) < deg( g , n+1 ) )
        {
            poly_swap(f , g, n+1);
            poly_swap(b , c, n+1);

            //printf("b and c swapped...\n");
            //dbgpoly(b);
            //dbgpoly(c);

            //getchar();
        }
        int g1=modinv(g[0],p);

        u = ( f[0] * g1 )%p;
        if(u<0) u+=p;


        poly_assign(gtemp , g , n+1);
        poly_assign(ctemp , c , n+1);
        //printf("After assign..\n");
        //dbgpoly(ctemp);

        poly_mul_int(gtemp , -1*u , n+1) ;
        poly_mul_int(ctemp , -1*u , n+1) ;
        //printf("After mul..\n");
        //dbgpoly(ctemp);

        poly_add(f , gtemp , n+1);
        poly_add(b , ctemp , n+1);
        //printf("After add..\n");

        poly_mod(f , n+1 , p);
        poly_mod(b , n+1 , p);

    }

    //printf("Inverse is: \n");
    //dbgpoly(b);
    poly_assign(poly , b , n+1);
    poly_mod(poly , n , p);




	return ;
}

//exponentiation via squaring
int ex( int p , int r)
{
    int ans=1;
    while(r>0)
    {
        if(r&1) ans*=p;
        r=r>>1;
        p*=p;
    }
    return ans;
}



/*-------------------------------------------------------
POLYNOMIAL inverse of poly(X) in the Field (Z/(p^r)Z)[X]/X^n -1
poly and invpoly are input polynomials.
Here invpoly is the inverse of poly  in Z/pZ which needs to be
calculated using poly_inverse before using this function.
p is the prime number and r is its exponent
 ----------------------------------------------------------*/
void poly_inverse_primepower( int poly[] , int invpoly[], int p , int r , int n)
{
    int product[maxx];
    int temp[maxx];

    poly_assign(product , poly , n+1) ;
    poly_init(temp , n+1) ;
    //dbgpoly(poly);
    temp[0] = 2;
    int q = p , e = ex( p , r);

    while(q < e)
    {
        q=q*q;
        poly_assign(product , poly , n+1);
        poly_mul_poly(product , invpoly , n, q);
        poly_mul_int (product ,  -1 , n+1);
        poly_add( product , temp , n+1);
        poly_mul_poly(invpoly , product , n , q);

        poly_mod(invpoly , n +1 , q);
        //poly_mul_poly( product ,  )

    }

    poly_assign(poly , invpoly , n+1);
}

//swap two integers


void poly_shuffle( int poly[] , int n)
{
    int j = 3;
    while(j--)
    {
        for(i = 0; i < n; i++ )
        {
            j=( i + rand() )% n;
            if( i == j ) j++;
            swp(poly[i] , poly[j]);
        }
    }
}

#endif // POLYRING
