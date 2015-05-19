#include <stdio.h>
#include <stdlib.h>
#include "polyring.h"
#define ll long long
const static int primes[168]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997};
int h[maxx];                                    // public key
int m[maxx];                                    // plaintext message
int e[maxx];                                    // encrypted message
int f[maxx],fp[maxx];                           // private key
void num_to_poly(ll message)
{
    int i=0;
    while(message>0)
    {
        m[i++]=message%3;
        message /= 3;
    }
}

ll poly_to_num(int poly[], int len)
{
    ll message=0, b=1, i;
    for(i = 0 ; i <= len ; i ++ ,  b *= 3 )message +=b * poly[i];
    return message;
}
void ntru_keygen(int n, int p, int q)
{
    //freopen("inputkeygen.txt","r",stdin);
    //freopen("output.txt","w",stdout);
    int g[maxx],fq[maxx],fg[maxx],testfp[maxx],testfq[maxx];
    int q1,qt, j, r, k;
    q1=q;
    poly_init(f,n+1);                 // initialize f polynomial
    poly_init(g,n+1);                 // initialize g polynomial
    poly_init(fp,n+1);
    poly_init(fq,n+1);
    poly_init(fg,n+1);
    poly_init(testfp,n+1);
    poly_init(testfq,n+1);

    fg[0] = -1;
    fg[n] = 1;

    k = rand()%(n/8) + n/4;

    for(i = 0 ; i < k ; i++)
        f[i] = 1;
    for( i = k; i < 2*k - 1; i++)
        f[i] = -1;

    poly_shuffle(f, n);
    //dbgpoly(f);

    for(i = 0 ; i < k ; i++)
        g[i] = 1;
    for( i = k; i < 2*k ; i++)
        g[i] = -1;

    poly_shuffle(g, n);
    //dbgpoly(g);
//    for(i=0;i<n;i++)
//        scanf("%d",&f[i]);
//    dbgpoly(f);
//
//    for(i=0;i<n;i++)
//       scanf("%d",&g[i]);
//
//    dbgpoly(g);

    //printf("\nCheck f(X) properly.. It should have m number of 1s and m-1 number of -1s..\nIf not infinite loop below...... \n");
    /*----------------------------------------------------

    Code for testing f




    -------------------------------------------------------*/
    poly_assign(fp,f,n+1);
    poly_assign(fq,f,n+1);
    poly_assign(testfq,f,n+1);

    poly_inverse(fp,fg,n,p);         // calculate fp as f inverse modulo p
    //dbgpoly(fp);


      //Calculating fq as inverse modulo q

    for( i = 0 ; i < 168 ; i++)
    {
        if(q1 % primes[i] ==0)
        {
            break;
        }
    }

    r=0;
    while(q1 % primes[i] ==0)
    {
        q1/=primes[i];
        r++;
    }
    qt=primes[i];
    poly_inverse( testfq , fg , n , qt );
    poly_inverse_primepower(fq , testfq, qt, r , n);
    poly_mod(fq, n, q);
    //dbgpoly(fq);

    /* Calculation of h      */
    poly_init(h,n+1);
    poly_assign(h, fq, n+1);
    poly_mul_poly(h, g, n, q);
    poly_mul_int(h, p, n);
    poly_mod(h,n,q);
    poly_ring_shift(h,q,n);
    dbgpoly(h);
    return ;
}

void ntru_encryption(int n, int q)
{
    //freopen("inputencryption.txt","r",stdin);
    int i, k;
    int r[maxx];
    ll message;
    poly_init(m,n+1);
    poly_init(r,n+1);
    printf("Enter value(length< 64) : ");
    scanf("%lld", &message);
    num_to_poly(message);
    poly_ring_shift(m, 3, n);
//    for(i=0;i<n;i++)
//        scanf("%d",&r[i]);
    k = rand()%(n/8) + n/4;

    for(i = 0 ; i < k ; i++)
        r[i] = 1;
    for( i = k; i < 2*k ; i++)
        r[i] = -1;

    poly_shuffle(r, n);

    dbgpoly(m);
    dbgpoly(r);
    poly_init(e,n+1);
    poly_assign(e,r,n);
    poly_mul_poly(r,h,n,q);
    poly_add(r,m,n);
    poly_mod(r,n,q);
    poly_assign(e,r,n);
    dbgpoly(e);
    return;
}

void ntru_decryption(int n, int p,int q)
{
    int a[maxx],b[maxx],c[maxx];
    ll dec_message;
    poly_init(a,n+1);
    poly_assign(a,e,n+1);
    poly_mul_poly(a,f,n,q);
    poly_mod(a,n,q);
    poly_ring_shift(a,q,n);
    //dbgpoly(a);

    poly_init(b,n+1);
    poly_assign(b,a,n+1);
    poly_mod(b,n,p);

    poly_init(c,n+1);
    poly_assign(c,fp,n+1);
    //dbgpoly(c);
    poly_mul_poly(c,b,n,p);
    poly_ring_shift(c,p,n);
    //dbgpoly(c);
    int len= deg(c, n);

    poly_mod(c, n, 3);
    dec_message = poly_to_num(c, len);
    dbgpoly(c);
    printf("Actual message is: %lld\n", dec_message);
    return;
}

int main()
{
    int n,p,q;
    //freopen("input.txt","r",stdin);
    printf("Enter security Parameters :\n");

    printf("N (should be prime ) : ");scanf("%d",&n);
    printf("p (should be small prime ) : ");scanf("%d",&p);
    printf("q (should be much larger than p (prime power/another prime) : ");scanf("%d",&q);
    ntru_keygen(n, p, q);
    ntru_encryption(n, q);
    ntru_decryption(n, p, q);
    return 0;
}

