#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "lfo.hpp"

using namespace lvtk;

Lfo::Lfo(double rate)
    : Plugin<Lfo>(p_n_ports)
{
    long tm;

    m_rate = rate;
    wave_period = rate / (16.0 * 5);

    trigger = false;
    si = 0;
    old_si = 0;
    sa = -1;
    old_sa = 0;
    t = 0;
    old_t = 0;
    r = 0;
    old_r = 0;
    old_sh = 0;
    sh = 0;
    state = 0;
    dt = 4.0 / wave_period;

    waveForm = SINUS;

    tm = time(NULL) % 1000000;
    srand(abs(tm - 10000 * (tm % 100)));
}

void Lfo::run(uint32_t nframes)
{
    int l2, k, len, phi0i, l2_out;
    double ldsi, ldsa, ldt, ldr, ldsh, dt0, dsa;

    waveForm = floor(*p(p_waveForm));

    wave_period = m_rate / (16.0 * *p(p_freq));
    dsa = 2.0 / wave_period;
    dt0 = 4.0 / wave_period;
    phi0i = (int)(*p(p_phi0) / 6.283 * wave_period);

    len = nframes;
    l2 = -1;
    l2_out = 0;
    do
    {
        k = (len > 24) ? 16 : len;
        l2 += k;
        if (!trigger && (p(p_reset)[l2] > 0.5))
        {
            trigger = true;
            t = 0;
            state = 0;
            dt = dt0;
            r = -1;
            si = 0;
        }
        if (trigger && (p(p_reset)[l2] < 0.5))
        {
            trigger = false;
        }

        if (t >= 1.0)
        {
            state = 1;
            dt = -dt0;
        }
        else if (t <= -1.0)
        {
            state = 3;
            dt = dt0;
        }
        else if ((state == 1) && (t < 0))
        {
            state = 2;
            r = 1;
            sh = 2.0 * (double)rand() / (double)RAND_MAX - 1.0;
        }
        else if ((state == 3) && (t > 0))
        {
            state = 0;
            r = -1;
            sh = 2.0 * (double)rand() / (double)RAND_MAX - 1.0;
            sa = -1;
        }

        si = (state < 2) ? t * (2.0 - t) : t * (2.0 + t);
        sa += dsa;
        t += dt;
        len -= k;
        ldsi = (si - old_si) / (double)k;
        ldsa = (sa - old_sa) / (double)k;
        ldsh = (sh - old_sh) / (double)k;
        ldt = (t - old_t) / (double)k;
        ldr = (r - old_r) / (double)k;
        while (k--)
        {
            old_si += ldsi;
            old_sa += ldsa;
            old_sh += ldsh;
            old_t += ldt;
            old_r += ldr;

            p(p_out_sine)[l2_out] = old_si;
            p(p_out_triangle)[l2_out] = old_t;
            p(p_out_saw_up)[l2_out] = old_sa;
            p(p_out_saw_down)[l2_out] = -old_sa;
            p(p_out_rectangle)[l2_out] = -old_r;
            p(p_out_sample_hold)[l2_out] = old_sh;

            l2_out++;
        }
    }
    while(len);
}

static int _ = Lfo::register_class("http://github.com/blablack/ams-lv2/lfo");
