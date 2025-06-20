/*
** SPDX-License-Identifier: BSD-3-Clause
** Copyright Contributors to the OpenEXR Project.
*/

#ifndef OPENEXR_CORE_UNPACK_H
#define OPENEXR_CORE_UNPACK_H

#include "openexr_config.h"
#include "openexr_decode.h"
#include "openexr_encode.h"

#include "internal_structs.h"

#if defined(__GNUC__)
#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#elif defined(__aarch64__) || defined(__arm__)
#include <arm_acle.h>
#else
#error Unknown Architecture
#endif
#elif defined(_MSC_VER)
#if defined(_M_X64) || defined(_M_IX86)
#include <immintrin.h>
#elif defined(_M_ARM64) || defined(_M_ARM)
#include <intrin.h>
#else
#error Unknown Architecture
#endif
#else
#error Unknown Compiler
#endif

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef exr_result_t (*internal_exr_unpack_fn) (exr_decode_pipeline_t*);

internal_exr_unpack_fn internal_exr_match_decode (
    exr_decode_pipeline_t* decode,
    int                    isdeep,
    int                    chanstofill,
    int                    chanstounpack,
    int                    sametype,
    int                    sameouttype,
    int                    samebpc,
    int                    sameoutbpc,
    int                    hassampling,
    int                    hastypechange,
    int                    sameoutinc,
    int                    simpinterleave,
    int                    simpinterleaverev,
    int                    simplineoff);

typedef exr_result_t (*internal_exr_pack_fn) (exr_encode_pipeline_t*);

internal_exr_pack_fn
internal_exr_match_encode (exr_encode_pipeline_t* encode, int isdeep);

exr_result_t internal_coding_fill_channel_info (
    exr_coding_channel_info_t** channels,
    int16_t*                    num_chans,
    exr_coding_channel_info_t*  builtinextras,
    const exr_chunk_info_t*     cinfo,
    exr_const_context_t         pctxt,
    exr_const_priv_part_t       part);

exr_result_t internal_coding_update_channel_info (
    exr_coding_channel_info_t* channels,
    int16_t                    num_chans,
    const exr_chunk_info_t*    cinfo,
    exr_const_context_t        pctxt,
    exr_const_priv_part_t      part);

exr_result_t internal_validate_next_chunk (
    exr_encode_pipeline_t* encode,
    exr_const_context_t    pctxt,
    exr_const_priv_part_t  part);

/**************************************/

exr_result_t internal_encode_free_buffer (
    exr_encode_pipeline_t*               encode,
    exr_transcoding_pipeline_buffer_id_t bufid,
    void**                               buf,
    size_t*                              sz);

exr_result_t internal_encode_alloc_buffer (
    exr_encode_pipeline_t*               encode,
    exr_transcoding_pipeline_buffer_id_t bufid,
    void**                               buf,
    size_t*                              cursz,
    size_t                               newsz);

exr_result_t internal_decode_free_buffer (
    exr_decode_pipeline_t*               decode,
    exr_transcoding_pipeline_buffer_id_t bufid,
    void**                               buf,
    size_t*                              sz);

exr_result_t internal_decode_alloc_buffer (
    exr_decode_pipeline_t*               decode,
    exr_transcoding_pipeline_buffer_id_t bufid,
    void**                               buf,
    size_t*                              cursz,
    size_t                               newsz);

/**************************************/

static inline float
half_to_float(uint16_t hv)
{
#if defined(__GNUC__)
#if defined(__x86_64__) || defined(__i386__)
    return _cvtsh_ss(hv);
#elif defined(__aarch64__) || defined(__arm__)
    return vgetq_lane_f32(vcvt_f32_f16(vreinterpret_f16_u16(vdup_n_u16(hv))), 0);
#else
#error Unknown Architecture
#endif
#elif defined(_MSC_VER)
#if defined(_M_X64) || defined(_M_IX86)
#if defined(__clang__)
    // CLANG-CL
    return _cvtsh_ss(hv);
#else
    // MSVC
    return _mm_cvtss_f32(_mm_cvtph_ps(_mm_set1_epi16(hv)));
#endif
#elif defined(_M_ARM64) || defined(_M_ARM)
    return vgetq_lane_f32(vcvt_f32_f16(vreinterpret_f16_u16(vdup_n_u16(hv))), 0);
#else
#error Unknown Architecture
#endif
#else
#error Unknown Compiler
#endif
}

static inline uint32_t
half_to_float_int (uint16_t hv)
{
    union
    {
        uint32_t i;
        float    f;
    } v;
    v.f = half_to_float (hv);
    return v.i;
}

static inline uint16_t
float_to_half(float fv)
{
#if defined(__GNUC__)
#if defined(__x86_64__) || defined(__i386__)
    return _cvtss_sh(fv, (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
#elif defined(__aarch64__) || defined(__arm__)
    return vget_lane_u16(vcvt_f16_f32(vdupq_n_f32(fv)), 0);
#else
#error Unknown Architecture
#endif
#elif defined(_MSC_VER)
#if defined(_M_X64) || defined(_M_IX86)
#if defined(__clang__)
    // CLANG-CL
    return _cvtss_sh(fv, (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
#else
    // MSVC
    return _mm_extract_epi16(_mm_cvtps_ph(_mm_set_ss(fv), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)), 0);
#endif
#elif defined(_M_ARM64) || defined(_M_ARM)
    return vget_lane_u16(vcvt_f16_f32(vdupq_n_f32(fv)), 0);
#else
#error Unknown Architecture
#endif
#else
#error Unknown Compiler
#endif
}

static inline uint16_t
float_to_half_int (uint32_t fiv)
{
    union
    {
        uint32_t i;
        float    f;
    } v;
    v.i = fiv;
    return float_to_half (v.f);
}

/**************************************/

static inline uint32_t
half_to_uint (uint16_t hv)
{
    /* replicating logic from imfmisc if negative or nan, return 0, if
     * inf, return uint32 max otherwise convert to float and cast to
     * uint */
    if (hv & 0x8000) return 0;
    if ((hv & 0x7c00) == 0x7c00)
    {
        if ((hv & 0x3ff) != 0) return 0;
        return UINT32_MAX;
    }
    return (uint32_t) (half_to_float (hv));
}

static inline uint32_t
float_to_uint (float fv)
{
    if (fv < 0.f || isnan (fv)) return 0;
    if (isinf (fv) || fv > (float) (UINT32_MAX)) return UINT32_MAX;
    return (uint32_t) (fv);
}

static inline uint32_t
float_to_uint_int (uint32_t fint)
{
    union
    {
        uint32_t i;
        float    f;
    } v;
    v.i = fint;
    return float_to_uint (v.f);
}

static inline uint16_t
uint_to_half (uint32_t ui)
{
    if (ui > 65504) return 0x7c00;

    return float_to_half ((float) (ui));
}

static inline float
uint_to_float (uint32_t ui)
{
    return (float) ui;
}

static inline uint32_t
uint_to_float_int (uint32_t ui)
{
    union
    {
        uint32_t i;
        float    f;
    } v;
    v.f = uint_to_float (ui);
    return v.i;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* OPENEXR_CORE_UNPACK_H */
