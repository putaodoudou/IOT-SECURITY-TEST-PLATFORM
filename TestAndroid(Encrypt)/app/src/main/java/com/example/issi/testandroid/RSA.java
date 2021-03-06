package com.example.issi.testandroid;

/**
 * Created by ISSI on 2017-08-21.
 */

public class RSA {

    public static final int MAX = 100;

    public int pq=0;
    public int n=0;
    public int e=0;

    // 소수인지 판단하는 함수
    boolean Is_Primes(int max)
    {
        int i = 0;
        boolean t = false;
        for (i = 2; i < max; i++)
        {
            if (max % i == 0) // 2부터 입력 받은 수까지 나눴을때 나머지가 0인 경우
            {
                t = false;	// 즉 나누어 떨어진다는 것은 소수아님
                break;
            }
            else
            {
                t = true;	// 나누어 떨어지지 않으면 약수가 1과 입력된 수 이므로 소수
            }
        }
        return t;
    }

    // 랜덤으로 소수만드는 함수
    int Create_Primes()
    {
        int p, q = 0;
        int n = 0;

        while (true)
        {
            while (true)
            {
                p = (int)(Math.random() * MAX) + 10;	//10자리 이상의 p를 선택

                if (Is_Primes(p))	//p가 소수인지 판단
                {
                    break;	//소수이면 빠져나옴
                }
            }
            while (true)
            {
                q = (int)(Math.random() * MAX) + 10;	//10자리 이상의 q를 선택

                if (Is_Primes(q))	//q가 소수인지 판단
                {
                    break;	//소수이면 빠져나옴
                }
            }
            if (p != q)	//p와 q가 일치하지않는 경우
                break;	//반복문에서 빠져나옴

        }

        pq = p*q;	//pq는 p, q의 곱
        n = (p - 1) * (q - 1);	//n을 오일러 피함수 n으로 정의
        p = 0;
        q = 0;	//p,q 초기화

        return n;
    }

    //e 를 정하는 함수
    int Select_e(int n)
    {
        int x = 0;
        int count = 0;
        int select = 0;
        int u = 0, v = 0, e = 0, t = 0;


        for (x = 2; x < n; x++)
        {
            u = n;
            v = x;

            while (v != 0) //유클리드 호제법을 이용한 최대공약수 구하는 알고리즘
            {
                t = u % v;
                u = v;
                v = t;
            }
            if (u == 1)	//최대 공약수가 1이면 서로소이므로 카운트를 증가
            {
                count++;
            }
        }

        select = (int)(Math.random() * count);	//서로소중 하나를 랜덤으로 선택하기 위해 카운트를 임의로 선택

        count = 0;
        v = 0;
        for (x = 2; x < n; x++)	//다시 서로소를 구함
        {
            u = n;
            v = x;

            while (v != 0)	//유클리드 호제법 알고리즘
            {
                t = u % v;
                u = v;
                v = t;
            }
            if (u == 1)	//서로소인 경우
            {
                count++;
                if (select == count)	//임의로 선택한 값과 카운트가 같으면
                {
                    e = x;	//e값을 선택하고 빠져나옴
                    break;
                }
            }
        }

        return e;
    }

    //d 를 정하는 함수
    int Select_d(int e, int n)
    {

        //mod
        int d = 0;

        while (true)
        {
            d++;

            if ((e * d) % n == 1)	// 모듈러 연산을 통해 d를 구함 ( ed(mod(오일러 피함수)) = 1 인 d를 구하는 과정 )
            {
                break;
            }
        }
        return d;

    }

    //암호화 함수
    int Encrypt(int text)
    {
        int temp = 1;
        int count = 0;

        n = Create_Primes();	// 오일러 피함수 구함

        e = Select_e(n);	// e를 선택

        while (true)
        {
            temp *= text;	// 평문 제곱
            temp %= pq;	//모듈러 연산

            count++;

            if (count == e)	// e만큼 실행하면 빠져나옴
                break;
        }

        return temp;
    }

    //복호화 함수
    int[] Decrypt(int[] text)
    {
        int d = 0;
        int count = 0;
        int[] temp = new int[16];

        for (int i = 0;i < 16; i++)
            temp[i] = 1;

        d = Select_d(e, n);	//d를 선택
        n = 0;

        for (int i = 0; i < 16; i++)
        {
            count = 0;
            while (true)
            {
                temp[i] *= text[i];	// 암호문 제곱
                temp[i] %= pq;	// 모듈러 연산

                count++;

                if (count == d)	// d만큼 실행하면 빠져나옴
                    break;
            }
        }
        return temp;

    }


}
