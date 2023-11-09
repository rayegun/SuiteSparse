//------------------------------------------------------------------------------
// CHOLMOD/MatrixOps/cholmod_norm: compute norm of a sparse matrix
//------------------------------------------------------------------------------

// CHOLMOD/MatrixOps Module.  Copyright (C) 2005-2023, Timothy A. Davis.
// All Rights Reserved.
// SPDX-License-Identifier: GPL-2.0+

//------------------------------------------------------------------------------

// r = norm (A), compute the infinity-norm, 1-norm, or 2-norm of a sparse or
// dense matrix.  Can compute the 2-norm only for a dense column vector.
// Returns -1 if an error occurs.
//
// Pattern, real, complex, and zomplex sparse matrices, with any dtype, are
// supported.

#include "cholmod_internal.h"

#ifndef NGPL
#ifndef NMATRIXOPS

//------------------------------------------------------------------------------
// t_cholmod_norm_worker template
//------------------------------------------------------------------------------

#define PATTERN
#include "t_cholmod_norm_worker.c"

#define DOUBLE
#define REAL
#include "t_cholmod_norm_worker.c"
#define COMPLEX
#include "t_cholmod_norm_worker.c"
#define ZOMPLEX
#include "t_cholmod_norm_worker.c"

#undef  DOUBLE
#define SINGLE
#define REAL
#include "t_cholmod_norm_worker.c"
#define COMPLEX
#include "t_cholmod_norm_worker.c"
#define ZOMPLEX
#include "t_cholmod_norm_worker.c"

//------------------------------------------------------------------------------
// cholmod_norm_dense
//------------------------------------------------------------------------------

double CHOLMOD(norm_dense)
(
    // input:
    cholmod_dense *X,   // matrix to compute the norm of
    int norm,           // type of norm: 0: inf. norm, 1: 1-norm, 2: 2-norm
    cholmod_common *Common
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    RETURN_IF_NULL_COMMON (EMPTY) ;
    RETURN_IF_NULL (X, EMPTY) ;
    RETURN_IF_XTYPE_INVALID (X, CHOLMOD_REAL, CHOLMOD_ZOMPLEX, EMPTY) ;
    Common->status = CHOLMOD_OK ;
    if (norm < 0 || norm > 2 || (norm == 2 && X->ncol > 1))
    {
        ERROR (CHOLMOD_INVALID, "invalid norm") ;
        return (EMPTY) ;
    }

    //--------------------------------------------------------------------------
    /* allocate workspace, if needed */
    //--------------------------------------------------------------------------

    double *W = NULL ;
    bool use_workspace = (norm == 0 && X->ncol > 4) ;
    if (use_workspace)
    {
        CHOLMOD(alloc_work) (0, 0, X->nrow, CHOLMOD_DOUBLE, Common) ;
        W = (double *) (Common->Xwork) ;
        if (Common->status < CHOLMOD_OK)
        {
            /* oops, no workspace */
            use_workspace = FALSE ;
            W = NULL ;
        }
    }

    //--------------------------------------------------------------------------
    /* compute the norm */
    //--------------------------------------------------------------------------

    double xnorm = 0 ;

    switch ((X->xtype + X->dtype) % 8)
    {

        case CHOLMOD_SINGLE + CHOLMOD_REAL:
            xnorm = r_s_cholmod_norm_dense_worker (X, norm, W) ;
            break ;

        case CHOLMOD_SINGLE + CHOLMOD_COMPLEX:
            xnorm = c_s_cholmod_norm_dense_worker (X, norm, W) ;
            break ;

        case CHOLMOD_SINGLE + CHOLMOD_ZOMPLEX:
            xnorm = z_s_cholmod_norm_dense_worker (X, norm, W) ;
            break ;

        case CHOLMOD_DOUBLE + CHOLMOD_REAL:
            xnorm = r_cholmod_norm_dense_worker (X, norm, W) ;
            break ;

        case CHOLMOD_DOUBLE + CHOLMOD_COMPLEX:
            xnorm = c_cholmod_norm_dense_worker (X, norm, W) ;
            break ;

        case CHOLMOD_DOUBLE + CHOLMOD_ZOMPLEX:
            xnorm = z_cholmod_norm_dense_worker (X, norm, W) ;
            break ;
    }

    return (xnorm) ;
}

//------------------------------------------------------------------------------
// cholmod_norm_sparse
//------------------------------------------------------------------------------

double CHOLMOD(norm_sparse)
(
    // input:
    cholmod_sparse *A,  // matrix to compute the norm of
    int norm,           // type of norm: 0: inf. norm, 1: 1-norm
    cholmod_common *Common
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    RETURN_IF_NULL_COMMON (EMPTY) ;
    RETURN_IF_NULL (A, EMPTY) ;
    RETURN_IF_XTYPE_INVALID (A, CHOLMOD_PATTERN, CHOLMOD_ZOMPLEX, EMPTY) ;
    Common->status = CHOLMOD_OK ;
    if (norm < 0 || norm > 1)
    {
        ERROR (CHOLMOD_INVALID, "invalid norm") ;
        return (EMPTY) ;
    }
    if (A->stype && A->nrow != A->ncol)
    {
        ERROR (CHOLMOD_INVALID, "matrix invalid") ;
        return (EMPTY) ;
    }

    //--------------------------------------------------------------------------
    // allocate workspace, if needed
    //--------------------------------------------------------------------------

    double *W = NULL ;
    if (A->stype || norm == 0)
    {
        CHOLMOD(alloc_work) (0, 0, A->nrow, CHOLMOD_DOUBLE, Common) ;
        if (Common->status < CHOLMOD_OK)
        {
            // out of memory
            return (EMPTY) ;
        }
        W = (double *) (Common->Xwork) ;
        DEBUG (for (Int i = 0 ; i < A->nrow ; i++) ASSERT (W [i] == 0)) ;
    }

    //--------------------------------------------------------------------------
    // compute the norm
    //--------------------------------------------------------------------------

    double anorm = 0 ;

    switch ((A->xtype + A->dtype) % 8)
    {

        default:
            anorm = p_cholmod_norm_sparse_worker (A, norm, W) ;
            break ;

        case CHOLMOD_SINGLE + CHOLMOD_REAL:
            anorm = r_s_cholmod_norm_sparse_worker (A, norm, W) ;
            break ;

        case CHOLMOD_SINGLE + CHOLMOD_COMPLEX:
            anorm = c_s_cholmod_norm_sparse_worker (A, norm, W) ;
            break ;

        case CHOLMOD_SINGLE + CHOLMOD_ZOMPLEX:
            anorm = z_s_cholmod_norm_sparse_worker (A, norm, W) ;
            break ;

        case CHOLMOD_DOUBLE + CHOLMOD_REAL:
            anorm = r_cholmod_norm_sparse_worker (A, norm, W) ;
            break ;

        case CHOLMOD_DOUBLE + CHOLMOD_COMPLEX:
            anorm = c_cholmod_norm_sparse_worker (A, norm, W) ;
            break ;

        case CHOLMOD_DOUBLE + CHOLMOD_ZOMPLEX:
            anorm = z_cholmod_norm_sparse_worker (A, norm, W) ;
            break ;
    }

    return (anorm) ;
}

#endif
#endif

