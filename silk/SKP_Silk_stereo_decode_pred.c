/***********************************************************************
Copyright (c) 2006-2011, Skype Limited. All rights reserved. 
Redistribution and use in source and binary forms, with or without 
modification, (subject to the limitations in the disclaimer below) 
are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright 
notice, this list of conditions and the following disclaimer in the 
documentation and/or other materials provided with the distribution.
- Neither the name of Skype Limited, nor the names of specific 
contributors, may be used to endorse or promote products derived from 
this software without specific prior written permission.
NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED 
BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF 
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

#include "SKP_Silk_main.h"

/* Decode mid/side predictors */
void SKP_Silk_stereo_decode_pred(
    ec_dec              *psRangeDec,                    /* I/O  Compressor data structure                   */
    SKP_int32           pred_Q13[]                      /* O    Predictors                                  */
)
{
    SKP_int   n, ibest[ 2 ] = { 0 }, jbest[ 2 ] = { 0 }, kbest[ 2 ];
    SKP_int32 low_Q13, step_Q13;

    /* Entropy decoding */
    n = ec_dec_icdf( psRangeDec, SKP_Silk_stereo_pred_joint_iCDF, 8 );
    kbest[ 0 ] = SKP_DIV32_16( n, 5 );
    kbest[ 1 ] = n - 5 * kbest[ 0 ];
    for( n = 0; n < 2; n++ ) {
        ibest[ n ] = ec_dec_icdf( psRangeDec, SKP_Silk_uniform3_iCDF, 8 );
        ibest[ n ] += 3 * kbest[ n ];
        jbest[ n ] = ec_dec_icdf( psRangeDec, SKP_Silk_uniform5_iCDF, 8 );
    }

    /* Dequantize */
    for( n = 0; n < 2; n++ ) {
        low_Q13 = SKP_Silk_stereo_pred_quant_Q13[ ibest[ n ] ];
        step_Q13 = SKP_SMULWB( SKP_Silk_stereo_pred_quant_Q13[ ibest[ n ] + 1 ] - low_Q13, 
            SKP_FIX_CONST( 0.5 / STEREO_QUANT_SUB_STEPS, 16 ) );
        pred_Q13[ n ] = SKP_SMLABB( low_Q13, step_Q13, 2 * jbest[ n ] + 1 );
    }

    /* Subtract second from first predictor (helps when actually applying these) */
    pred_Q13[ 0 ] -= pred_Q13[ 1 ];
}
