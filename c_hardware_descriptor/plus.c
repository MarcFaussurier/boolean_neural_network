# include "circuit.h"
///////////////////////////////////////
circuit     test 
(
    entrée      a,
    sortie      r
)
{
    si (a == 0)
    {
        *r = 1;
    }
    sinon 
    {
        *r = 0;
    }
}



circuit     plus
(
    entrée      a,
    entrée      b,
    entrée      c,

    sortie      s,
    sortie      r
)
{
    *s = c oux (a oux b);
    test(a*b + b*c + a*c, r);
    test(*r, r);
}




circuit     principal()
{
    registre a = 1, b = 1, c = 0; 
    registre s, r;

    plus(a,b,c, &s,&r);
    printf("s=%i r=%i\n", !!s, !!r);
}