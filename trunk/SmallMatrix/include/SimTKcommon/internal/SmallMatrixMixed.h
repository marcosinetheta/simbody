#ifndef SimTK_SIMMATRIX_SMALLMATRIX_MIXED_H_
#define SimTK_SIMMATRIX_SMALLMATRIX_MIXED_H_

/* -------------------------------------------------------------------------- *
 *                      SimTK Core: SimTK Simmatrix(tm)                       *
 * -------------------------------------------------------------------------- *
 * This is part of the SimTK Core biosimulation toolkit originating from      *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2005-9 Stanford University and the Authors.         *
 * Authors: Michael Sherman                                                   *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

/**@file
 * This file defines global functions and class members which use a mix 
 * of Vec, Row, and Mat types and hence need to wait until everything is 
 * defined. Some of them may depend on Lapack also.
 */

namespace SimTK {

    // DOT PRODUCT

// Dot product and corresponding infix operator*(). Note that
// the '*' operator is just a matrix multiply so is strictly 
// row*column to produce a scalar (1x1) result.
//
// In keeping with Matlab precedent, however, the explicit dot()
// function is defined on two column vectors
// v and w such that dot(v,w)= ~v * w. That means we use the
// Hermitian transpose of the elements of v, and the elements of
// w unchanged. If v and/or w are rows, we first convert them
// to vectors via *positional* transpose. So no matter what shape
// these have on the way in it is always the Hermitian transpose
// (or complex conjugate for scalars) of the first item times
// the unchanged elements of the second item.


template <int M, class E1, int S1, class E2, int S2> inline
typename CNT<typename CNT<E1>::THerm>::template Result<E2>::Mul 
dot(const Vec<M,E1,S1>& r, const Vec<M,E2,S2>& v) {
    typename CNT<typename CNT<E1>::THerm>::template Result<E2>::Mul sum(dot(reinterpret_cast<const Vec<M-1,E1,S1>&>(r), reinterpret_cast<const Vec<M-1,E2,S2>&>(v)) + CNT<E1>::transpose(r[M-1])*v[M-1]);
    return sum;
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<typename CNT<E1>::THerm>::template Result<E2>::Mul 
dot(const Vec<1,E1,S1>& r, const Vec<1,E2,S2>& v) {
    typename CNT<typename CNT<E1>::THerm>::template Result<E2>::Mul sum(CNT<E1>::transpose(r[0])*v[0]);
    return sum;
}

// dot product (row * conforming vec)
template <int N, class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul 
operator*(const Row<N,E1,S1>& r, const Vec<N,E2,S2>& v) {
    typename CNT<E1>::template Result<E2>::Mul sum(reinterpret_cast<const Row<N-1,E1,S1>&>(r)*reinterpret_cast<const Vec<N-1,E2,S2>&>(v) + r[N-1]*v[N-1]);
    return sum;
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul 
operator*(const Row<1,E1,S1>& r, const Vec<1,E2,S2>& v) {
    typename CNT<E1>::template Result<E2>::Mul sum(r[0]*v[0]);
    return sum;
}

// Alternate acceptable forms for dot().
template <int N, class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul 
dot(const Row<N,E1,S1>& r, const Vec<N,E2,S2>& v) {
    return dot(r.positionalTranspose(),v);
}
template <int M, class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul 
dot(const Vec<M,E1,S1>& v, const Row<M,E2,S2>& r) {
    return dot(v,r.positionalTranspose());
}
template <int N, class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul 
dot(const Row<N,E1,S1>& r, const Row<N,E2,S2>& s) {
    return dot(r.positionalTranspose(),s.positionalTranspose());
}

    // OUTER PRODUCT

// Outer product and corresponding infix operator*(). Note that
// the '*' operator is just a matrix multiply so is strictly 
// column_mX1 * row_1Xm to produce an mXm matrix result.
//
// Although Matlab doesn't provide outer(), to be consistent
// we'll treat it as discussed for dot() above. That is, outer
// is defined on two column vectors
// v and w such that outer(v,w)= v * ~w. That means we use the
// elements of v unchanged but use the Hermitian transpose of
// the elements of w. If v and/or w are rows, we first convert them
// to vectors via *positional* transpose. So no matter what shape
// these have on the way in it is always the unchanged elements of
// the first item times the Hermitian transpose (or complex
// conjugate for scalars) of the elements of the second item.

template <int M, class E1, int S1, class E2, int S2> inline
Mat<M,M, typename CNT<E1>::template Result<typename CNT<E2>::THerm>::Mul>
outer(const Vec<M,E1,S1>& v, const Vec<M,E2,S2>& w) {
    Mat<M,M, typename CNT<E1>::template Result<typename CNT<E2>::THerm>::Mul> m;
    for (int i=0; i<M; ++i)
        m[i] = v[i] * ~w;
    return m;
}

// This is the general conforming case of Vec*Row (outer product)
template <int M, class E1, int S1, class E2, int S2> inline
typename Vec<M,E1,S1>::template Result<Row<M,E2,S2> >::Mul
operator*(const Vec<M,E1,S1>& v, const Row<M,E2,S2>& r) {
    return Vec<M,E1,S1>::template Result<Row<M,E2,S2> >::MulOp::perform(v,r);
}


// Alternate acceptable forms for outer().
template <int M, class E1, int S1, class E2, int S2> inline
Mat<M,M, typename CNT<E1>::template Result<E2>::Mul>
outer(const Vec<M,E1,S1>& v, const Row<M,E2,S2>& r) {
    return outer(v,r.positionalTranspose());
}
template <int M, class E1, int S1, class E2, int S2> inline
Mat<M,M, typename CNT<E1>::template Result<E2>::Mul>
outer(const Row<M,E1,S1>& r, const Vec<M,E2,S2>& v) {
    return outer(r.positionalTranspose(),v);
}
template <int M, class E1, int S1, class E2, int S2> inline
Mat<M,M, typename CNT<E1>::template Result<E2>::Mul>
outer(const Row<M,E1,S1>& r, const Row<M,E2,S2>& s) {
    return outer(r.positionalTranspose(),s.positionalTranspose());
}

    // MAT*VEC, ROW*MAT (conforming)

// vec = mat * vec (conforming)
template <int M, int N, class ME, int CS, int RS, class E, int S> inline
typename Mat<M,N,ME,CS,RS>::template Result<Vec<N,E,S> >::Mul
operator*(const Mat<M,N,ME,CS,RS>& m,const Vec<N,E,S>& v) {
    typename Mat<M,N,ME,CS,RS>::template Result<Vec<N,E,S> >::Mul result;
    for (int i=0; i<M; ++i)
        result[i] = m[i]*v;
    return result;
}

// row = row * mat (conforming)
template <int M, class E, int S, int N, class ME, int CS, int RS> inline
typename Row<M,E,S>::template Result<Mat<M,N,ME,CS,RS> >::Mul
operator*(const Row<M,E,S>& r, const Mat<M,N,ME,CS,RS>& m) {
    typename Row<M,E,S>::template Result<Mat<M,N,ME,CS,RS> >::Mul result;
    for (int i=0; i<N; ++i)
        result[i] = r*m(i);
    return result;
}

    // SYMMAT*VEC, ROW*SYMMAT (conforming)

// vec = sym * vec (conforming)
template <int N, class ME, int RS, class E, int S> inline
typename SymMat<N,ME,RS>::template Result<Vec<N,E,S> >::Mul
operator*(const SymMat<N,ME,RS>& m,const Vec<N,E,S>& v) {
    typename SymMat<N,ME,RS>::template Result<Vec<N,E,S> >::Mul result;
    for (int i=0; i<N; ++i) {
        result[i] = m.getDiag()[i]*v[i];
        for (int j=0; j<i; ++j)
            result[i] += m.getEltLower(i,j)*v[j];
        for (int j=i+1; j<N; ++j)
            result[i] += m.getEltUpper(i,j)*v[j];
    }
    return result;
}

// Unroll loops for small cases.

// 1 flop.
template <class ME, int RS, class E, int S> inline
typename SymMat<1,ME,RS>::template Result<Vec<1,E,S> >::Mul
operator*(const SymMat<1,ME,RS>& m,const Vec<1,E,S>& v) {
    typename SymMat<1,ME,RS>::template Result<Vec<1,E,S> >::Mul result;
    result[0] = m.getDiag()[0]*v[0];
    return result;
}

// 6 flops.
template <class ME, int RS, class E, int S> inline
typename SymMat<2,ME,RS>::template Result<Vec<2,E,S> >::Mul
operator*(const SymMat<2,ME,RS>& m,const Vec<2,E,S>& v) {
    typename SymMat<2,ME,RS>::template Result<Vec<2,E,S> >::Mul result;
    result[0] = m.getDiag()[0]    *v[0] + m.getEltUpper(0,1)*v[1];
    result[1] = m.getEltLower(1,0)*v[0] + m.getDiag()[1]    *v[1];
    return result;
}

// 15 flops.
template <class ME, int RS, class E, int S> inline
typename SymMat<3,ME,RS>::template Result<Vec<3,E,S> >::Mul
operator*(const SymMat<3,ME,RS>& m,const Vec<3,E,S>& v) {
    typename SymMat<3,ME,RS>::template Result<Vec<3,E,S> >::Mul result;
    result[0] = m.getDiag()[0]    *v[0] + m.getEltUpper(0,1)*v[1] + m.getEltUpper(0,2)*v[2];
    result[1] = m.getEltLower(1,0)*v[0] + m.getDiag()[1]    *v[1] + m.getEltUpper(1,2)*v[2];
    result[2] = m.getEltLower(2,0)*v[0] + m.getEltLower(2,1)*v[1] + m.getDiag()[2]    *v[2];
    return result;
}

// row = row * sym (conforming)
template <int M, class E, int S, class ME, int RS> inline
typename Row<M,E,S>::template Result<SymMat<M,ME,RS> >::Mul
operator*(const Row<M,E,S>& r, const SymMat<M,ME,RS>& m) {
    typename Row<M,E,S>::template Result<SymMat<M,ME,RS> >::Mul result;
    for (int j=0; j<M; ++j) {
        result[j] = r[j]*m.getDiag()[j];
        for (int i=0; i<j; ++i)
            result[j] += r[i]*m.getEltUpper(i,j);
        for (int i=j+1; i<M; ++i)
            result[j] += r[i]*m.getEltLower(i,j);
    }
    return result;
}

// Unroll loops for small cases.

// 1 flop.
template <class E, int S, class ME, int RS> inline
typename Row<1,E,S>::template Result<SymMat<1,ME,RS> >::Mul
operator*(const Row<1,E,S>& r, const SymMat<1,ME,RS>& m) {
    typename Row<1,E,S>::template Result<SymMat<1,ME,RS> >::Mul result;
    result[0] = r[0]*m.getDiag()[0];
    return result;
}

// 6 flops.
template <class E, int S, class ME, int RS> inline
typename Row<2,E,S>::template Result<SymMat<2,ME,RS> >::Mul
operator*(const Row<2,E,S>& r, const SymMat<2,ME,RS>& m) {
    typename Row<2,E,S>::template Result<SymMat<2,ME,RS> >::Mul result;
    result[0] = r[0]*m.getDiag()[0]     + r[1]*m.getEltLower(1,0);
    result[1] = r[0]*m.getEltUpper(0,1) + r[1]*m.getDiag()[1];
    return result;
}

// 15 flops.
template <class E, int S, class ME, int RS> inline
typename Row<3,E,S>::template Result<SymMat<3,ME,RS> >::Mul
operator*(const Row<3,E,S>& r, const SymMat<3,ME,RS>& m) {
    typename Row<3,E,S>::template Result<SymMat<3,ME,RS> >::Mul result;
    result[0] = r[0]*m.getDiag()[0]     + r[1]*m.getEltLower(1,0) + r[2]*m.getEltLower(2,0);
    result[1] = r[0]*m.getEltUpper(0,1) + r[1]*m.getDiag()[1]     + r[2]*m.getEltLower(2,1);
    result[2] = r[0]*m.getEltUpper(0,2) + r[1]*m.getEltUpper(1,2) + r[2]*m.getDiag()[2];
    return result;
}

    // NONCONFORMING MULTIPLY

    // Nonconforming: Vec on left: v*r v*m v*sym v*v
    // Result has the shape of the "most composite" (deepest) argument.

// elementwise multiply (vec * nonconforming row)
template <int M, class E1, int S1, int N, class E2, int S2> inline
typename Vec<M,E1,S1>::template Result<Row<N,E2,S2> >::MulNon
operator*(const Vec<M,E1,S1>& v, const Row<N,E2,S2>& r) {
    return Vec<M,E1,S1>::template Result<Row<N,E2,S2> >::MulOpNonConforming::perform(v,r);
}
// elementwise multiply (vec * nonconforming mat)
template <int M, class E1, int S1, int MM, int NN, class E2, int CS2, int RS2> inline
typename Vec<M,E1,S1>::template Result<Mat<MM,NN,E2,CS2,RS2> >::MulNon
operator*(const Vec<M,E1,S1>& v, const Mat<MM,NN,E2,CS2,RS2>& m) {
    return Vec<M,E1,S1>::template Result<Mat<MM,NN,E2,CS2,RS2> >
                ::MulOpNonConforming::perform(v,m);
}
// elementwise multiply (vec * nonconforming symmat)
template <int M, class E1, int S1, int MM, class E2, int RS2> inline
typename Vec<M,E1,S1>::template Result<SymMat<MM,E2,RS2> >::MulNon
operator*(const Vec<M,E1,S1>& v, const SymMat<MM,E2,RS2>& m) {
    return Vec<M,E1,S1>::template Result<SymMat<MM,E2,RS2> >
                ::MulOpNonConforming::perform(v,m);
}
// elementwise multiply (vec * nonconforming vec)
template <int M, class E1, int S1, int MM, class E2, int S2> inline
typename Vec<M,E1,S1>::template Result<Vec<MM,E2,S2> >::MulNon
operator*(const Vec<M,E1,S1>& v1, const Vec<MM,E2,S2>& v2) {
    return Vec<M,E1,S1>::template Result<Vec<MM,E2,S2> >
                ::MulOpNonConforming::perform(v1,v2);
}

    // Nonconforming: Row on left -- r*v r*r r*m r*sym


// (row or mat) = row * mat (nonconforming)
template <int M, class E, int S, int MM, int NN, class ME, int CS, int RS> inline
typename Row<M,E,S>::template Result<Mat<MM,NN,ME,CS,RS> >::MulNon
operator*(const Row<M,E,S>& r, const Mat<MM,NN,ME,CS,RS>& m) {
    return Row<M,E,S>::template Result<Mat<MM,NN,ME,CS,RS> >
        ::MulOpNonConforming::perform(r,m);
}
// (row or vec) = row * vec (nonconforming)
template <int N, class E1, int S1, int M, class E2, int S2> inline
typename Row<N,E1,S1>::template Result<Vec<M,E2,S2> >::MulNon
operator*(const Row<N,E1,S1>& r, const Vec<M,E2,S2>& v) {
    return Row<N,E1,S1>::template Result<Vec<M,E2,S2> >
        ::MulOpNonConforming::perform(r,v);
}
// (row1 or row2) = row1 * row2(nonconforming)
template <int N1, class E1, int S1, int N2, class E2, int S2> inline
typename Row<N1,E1,S1>::template Result<Row<N2,E2,S2> >::MulNon
operator*(const Row<N1,E1,S1>& r1, const Row<N2,E2,S2>& r2) {
    return Row<N1,E1,S1>::template Result<Row<N2,E2,S2> >
        ::MulOpNonConforming::perform(r1,r2);
}

    // Nonconforming: Mat on left -- m*v m*r m*sym

// (mat or vec) = mat * vec (nonconforming)
template <int M, int N, class ME, int CS, int RS, int MM, class E, int S> inline
typename Mat<M,N,ME,CS,RS>::template Result<Vec<MM,E,S> >::MulNon
operator*(const Mat<M,N,ME,CS,RS>& m,const Vec<MM,E,S>& v) {
    return Mat<M,N,ME,CS,RS>::template Result<Vec<MM,E,S> >
        ::MulOpNonConforming::perform(m,v);
}
// (mat or row) = mat * row (nonconforming)
template <int M, int N, class ME, int CS, int RS, int NN, class E, int S> inline
typename Mat<M,N,ME,CS,RS>::template Result<Row<NN,E,S> >::MulNon
operator*(const Mat<M,N,ME,CS,RS>& m,const Row<NN,E,S>& r) {
    return Mat<M,N,ME,CS,RS>::template Result<Row<NN,E,S> >
        ::MulOpNonConforming::perform(m,r);
}

// (mat or sym) = mat * sym (nonconforming)
template <int M, int N, class ME, int CS, int RS, int Dim, class E, int S> inline
typename Mat<M,N,ME,CS,RS>::template Result<SymMat<Dim,E,S> >::MulNon
operator*(const Mat<M,N,ME,CS,RS>& m,const SymMat<Dim,E,S>& sy) {
    return Mat<M,N,ME,CS,RS>::template Result<SymMat<Dim,E,S> >
        ::MulOpNonConforming::perform(m,sy);
}

    // CROSS PRODUCT

// Cross product and corresponding operator%, but only for 2- and 3-Vecs
// and Rows. It is OK to mix same-length Vecs & Rows; cross product is
// defined elementwise and never transposes the individual elements.
//
// We also define crossMat(v) below which produces a 2x2 or 3x3
// matrix M such that M*w = v % w for w the same length vector (or row) as v.
// TODO: the 3d crossMat is skew symmetric but currently there is no
// SkewMat class defined so we have to return a full 3x3.

// For 3d cross product, we'll follow Matlab and make the return type a
// Row if either or both arguments are Rows, although I'm not sure that's
// a good idea! Note that the definition of crossMat means crossMat(r)*v
// will yield a column, while r%v is a Row.

template <class E1, int S1, class E2, int S2> inline
Vec<3,typename CNT<E1>::template Result<E2>::Mul>
cross(const Vec<3,E1,S1>& a, const Vec<3,E2,S2>& b) {
    return Vec<3,typename CNT<E1>::template Result<E2>::Mul>
        (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}
template <class E1, int S1, class E2, int S2> inline
Vec<3,typename CNT<E1>::template Result<E2>::Mul>
operator%(const Vec<3,E1,S1>& a, const Vec<3,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
cross(const Vec<3,E1,S1>& a, const Row<3,E2,S2>& b) {
    return Row<3,typename CNT<E1>::template Result<E2>::Mul>
        (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}
template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
operator%(const Vec<3,E1,S1>& a, const Row<3,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
cross(const Row<3,E1,S1>& a, const Vec<3,E2,S2>& b) {
    return Row<3,typename CNT<E1>::template Result<E2>::Mul>
        (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}
template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
operator%(const Row<3,E1,S1>& a, const Vec<3,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
cross(const Row<3,E1,S1>& a, const Row<3,E2,S2>& b) {
    return Row<3,typename CNT<E1>::template Result<E2>::Mul>
        (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}
template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
operator%(const Row<3,E1,S1>& a, const Row<3,E2,S2>& b) {return cross(a,b);}

// 2d cross product just returns a scalar. This is the z-value you
// would get if the arguments were treated as 3-vectors with 0
// z components.

template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
cross(const Vec<2,E1,S1>& a, const Vec<2,E2,S2>& b) {
    return a[0]*b[1]-a[1]*b[0];
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
operator%(const Vec<2,E1,S1>& a, const Vec<2,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
cross(const Row<2,E1,S1>& a, const Vec<2,E2,S2>& b) {
    return a[0]*b[1]-a[1]*b[0];
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
operator%(const Row<2,E1,S1>& a, const Vec<2,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
cross(const Vec<2,E1,S1>& a, const Row<2,E2,S2>& b) {
    return a[0]*b[1]-a[1]*b[0];
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
operator%(const Vec<2,E1,S1>& a, const Row<2,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
cross(const Row<2,E1,S1>& a, const Row<2,E2,S2>& b) {
    return a[0]*b[1]-a[1]*b[0];
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
operator%(const Row<2,E1,S1>& a, const Row<2,E2,S2>& b) {return cross(a,b);}

    // CROSS MAT

/// Calculate matrix M(v) such that M(v)*w = v % w. We produce the
/// same M regardless of whether v is a column or row.
/// Requires 3 flops to form.
template <class E, int S> inline
Mat<3,3,E>
crossMat(const Vec<3,E,S>& v) {
    return Mat<3,3,E>(Row<3,E>( E(0), -v[2],  v[1]),
                      Row<3,E>( v[2],  E(0), -v[0]),
                      Row<3,E>(-v[1],  v[0],  E(0)));
}
/// Specialize crossMat() for negated scalar types. Returned matrix loses negator.
/// Requires 3 flops to form.
template <class E, int S> inline
Mat<3,3,E>
crossMat(const Vec<3,negator<E>,S>& v) {
    // Here the "-" operators are just recasts, but the casts
    // to type E have to perform floating point negations.
    return Mat<3,3,E>(Row<3,E>( E(0),   -v[2],    E(v[1])),
                      Row<3,E>( E(v[2]), E(0),   -v[0]),
                      Row<3,E>(-v[1],    E(v[0]), E(0)));
}

/// Form cross product matrix from a Row vector; 3 flops.
template <class E, int S> inline
Mat<3,3,E> crossMat(const Row<3,E,S>& r) {return crossMat(r.positionalTranspose());}
/// Form cross product matrix from a Row vector whose elements are negated scalars; 3 flops.
template <class E, int S> inline
Mat<3,3,E> crossMat(const Row<3,negator<E>,S>& r) {return crossMat(r.positionalTranspose());}

/// Calculate 2D cross product matrix M(v) such that M(v)*w = v0*w1-v1*w0 = v % w (a scalar). 
/// Whether v is a Vec<2> or Row<2> we create the same M, which will be a 2-element Row.
/// Requires 1 flop to form.
template <class E, int S> inline
Row<2,E> crossMat(const Vec<2,E,S>& v) {
    return Row<2,E>(-v[1], v[0]);
}
/// Specialize 2D cross product matrix for negated scalar types; 1 flop.
template <class E, int S> inline
Row<2,E> crossMat(const Vec<2,negator<E>,S>& v) {
    return Row<2,E>(-v[1], E(v[0]));
}

/// Form 2D cross product matrix from a Row<2>; 1 flop.
template <class E, int S> inline
Row<2,E> crossMat(const Row<2,E,S>& r) {return crossMat(r.positionalTranspose());}
/// Form 2D cross product matrix from a Row<2> with negated scalar elements; 1 flop.
template <class E, int S> inline
Row<2,E> crossMat(const Row<2,negator<E>,S>& r) {return crossMat(r.positionalTranspose());}

    // CROSS MAT SQ

/// Calculate matrix S(v) such that S(v)*w = -v % (v % w) = (v%w)%v. S is a symmetric,
/// 3x3 matrix with nonnegative diagonals. If M(v) = crossMat(v), then 
/// S(v) = square(M(v)) = ~M(v)*M(v) = -M(v)*M(v) since M is skew symmetric.
/// Also, S(v) = dot(v,v)*I - outer(v,v) = ~v*v*I - v*~v, where I is the identity
/// matrix. This is the form necessary for shifting inertia matrices using
/// the parallel axis theorem, something we do a lot of in multibody dynamics.
/// Consequently we want to calculate S very efficiently, which we can do because
/// it has the following very simple form. Assume v=[x y z]. Then
/// <pre>
///          y^2+z^2      T         T
///  S(v) =    -xy     x^2+z^2      T
///            -xz       -yz     x^2+y^2
/// </pre>
/// where "T" indicates that the element is identical to the symmetric one.
/// This requires 11 flops to form.
/// We produce the same S(v) regardless of whether v is a column or row.
/// Note that there is no 2D equivalent of this operator.
template <class E, int S> inline
SymMat<3,E>
crossMatSq(const Vec<3,E,S>& v) {
    const E xx = square(v[0]);
    const E yy = square(v[1]);
    const E zz = square(v[2]);
    const E nx = -v[0];
    const E ny = -v[1];
    return SymMat<3,E>( yy+zz,
                        nx*v[1], xx+zz,
                        nx*v[2], ny*v[2], xx+yy );
}
// Specialize above for negated types. Returned matrix loses negator.
// The total number of flops here is the same as above: 11 flops.
template <class E, int S> inline
SymMat<3,E>
crossMatSq(const Vec<3,negator<E>,S>& v) {
    const E xx = square(v[0]);
    const E yy = square(v[1]);
    const E zz = square(v[2]);
    const E y = v[1]; // requires a negation to convert to E
    const E z = v[2];
    // The negations in the arguments below are not floating point
    // operations because the element type is already negated.
    return SymMat<3,E>( yy+zz,
                        -v[0]*y,  xx+zz,
                        -v[0]*z, -v[1]*z, xx+yy );
}

template <class E, int S> inline
SymMat<3,E> crossMatSq(const Row<3,E,S>& r) {return crossMatSq(r.positionalTranspose());}
template <class E, int S> inline
SymMat<3,E> crossMatSq(const Row<3,negator<E>,S>& r) {return crossMatSq(r.positionalTranspose());}


    // DETERMINANT

/// Special case 1x1 determinant. No computation.
template <class E, int CS, int RS> inline
const E& det(const Mat<1,1,E,CS,RS>& m) {
    return m(0,0);
}

/// Special case 2x2 determinant. 3 flops.
template <class E, int CS, int RS> inline
E det(const Mat<2,2,E,CS,RS>& m) {
    return E(m(0,0)*m(1,1) - m(0,1)*m(1,0));
}

/// Special case 3x3 determinant. 14 flops.
template <class E, int CS, int RS> inline
E det(const Mat<3,3,E,CS,RS>& m) {
    return E(  m(0,0)*(m(1,1)*m(2,2)-m(1,2)*m(2,1))
             - m(0,1)*(m(1,0)*m(2,2)-m(1,2)*m(2,0))
             + m(0,2)*(m(1,0)*m(2,1)-m(1,1)*m(2,0)));
}

/// Calculate the determinant of a square matrix larger than 3x3
/// by recursive template expansion. The matrix elements must be 
/// multiplication compatible for this to compile successfully. 
/// All scalar element types are acceptable; some composite types 
/// will also work but the result is probably meaningless.
/// The determinant suffers from increasingly bad scaling as the
/// matrices get bigger; you should consider an alternative if
/// possible (see Golub and van Loan, "Matrix Computations").
/// This uses M*det(M-1) + 4*M flops. For 4x4 that's 60 flops,
/// for 5x5 it's 320, and it grows really fast from there!
/// TODO: this is not the right way to calculate determinant --
/// should calculate LU factorization at 2/3 n^3 flops, then
/// determinant is product of LU's diagonals.
template <int M, class E, int CS, int RS> inline
E det(const Mat<M,M,E,CS,RS>& m) {
    typename CNT<E>::StdNumber sign(1);
    E                          result(0);
    // We're always going to drop the first row.
    const Mat<M-1,M,E,CS,RS>& m2 = m.template getSubMat<M-1,M>(1,0);
    for (int j=0; j < M; ++j) {
        // det() here is recursive but terminates at 3x3 above.
        result += sign*m(0,j)*det(m2.dropCol(j));
        sign = -sign;
    }
    return result;
}

    // INVERSE

/// Specialized 1x1 lapackInverse(): costs one divide.
template <class E, int CS, int RS> inline
typename Mat<1,1,E,CS,RS>::TInvert lapackInverse(const Mat<1,1,E,CS,RS>& m) {
    typedef typename Mat<1,1,E,CS,RS>::TInvert MInv;
    return MInv( E(typename CNT<E>::StdNumber(1)/m(0,0)) );
}

/// General inverse of small, fixed-size, square (mXm), non-singular matrix with 
/// scalar elements: use Lapack's LU routine with pivoting. This will only work 
/// if the element type E is a scalar type, although negator<> and conjugate<> 
/// are OK. This routine is <em>not</em> specialized for small matrix sizes other
/// than 1x1, while the inverse() method is specialized for other small sizes
/// for speed, possibly losing some numerical stability. The inverse() function
/// ends up calling this one at sizes for which it has no specialization. 
/// Normally you should call inverse(), but you can call lapackInverse() 
/// explicitly if you want to ensure that the most stable algorithm is used.
/// @see inverse()
template <int M, class E, int CS, int RS> inline
typename Mat<M,M,E,CS,RS>::TInvert lapackInverse(const Mat<M,M,E,CS,RS>& m) {
    // Copy the source matrix, which has arbitrary row and column spacing,
    // into the type for its inverse, which must be dense, columnwise
    // storage. That means its column spacing will be M and row spacing
    // will be 1, as Lapack expects for a Full matrix.
    typename Mat<M,M,E,CS,RS>::TInvert inv = m; // dense, columnwise storage

    // We'll perform the inversion ignoring negation and conjugation altogether, 
    // but the TInvert Mat type negates or conjugates the result. And because 
    // of the famous Sherman-Eastman theorem, we know that 
    // conj(inv(m))==inv(conj(m)), and of course -inv(m)==inv(-m).
    typedef typename CNT<E>::StdNumber Raw;   // Raw is E without negator or conjugate.
    Raw* rawData = reinterpret_cast<Raw*>(&inv(0,0));
    int ipiv[M], info;

    // This replaces "inv" with its LU facorization and pivot matrix P, such that
    // P*L*U = m (ignoring negation and conjugation).
    Lapack::getrf<Raw>(M,M,rawData,M,&ipiv[0],info);
    SimTK_ASSERT1(info>=0, "Argument %d to Lapack getrf routine was bad", -info);
    SimTK_ERRCHK1_ALWAYS(info==0, "lapackInverse(Mat<>)",
        "Matrix is singular so can't be inverted (Lapack getrf info=%d).", info);

    // The workspace size must be at least M. For larger matrices, Lapack wants
    // to do factorization in blocks of size NB in which case the workspace should
    // be M*NB. However, we will assume that this matrix is small (in the sense
    // that it fits entirely in cache) so the minimum workspace size is fine and
    // we don't need an extra getri call to find the workspace size nor any heap
    // allocation.

    Raw work[M];
    Lapack::getri<Raw>(M,rawData,M,&ipiv[0],&work[0],M,info);
    SimTK_ASSERT1(info>=0, "Argument %d to Lapack getri routine was bad", -info);
    SimTK_ERRCHK1_ALWAYS(info==0, "lapackInverse(Mat<>)",
        "Matrix is singular so can't be inverted (Lapack getri info=%d).", info);
    return inv;
}


/// Specialized 1x1 inverse: costs one divide.
template <class E, int CS, int RS> inline
typename Mat<1,1,E,CS,RS>::TInvert inverse(const Mat<1,1,E,CS,RS>& m) {
    typedef typename Mat<1,1,E,CS,RS>::TInvert MInv;
    return MInv( E(typename CNT<E>::StdNumber(1)/m(0,0)) );
}

/// Specialized 2x2 inverse: costs one divide plus 9 flops.
template <class E, int CS, int RS> inline
typename Mat<2,2,E,CS,RS>::TInvert inverse(const Mat<2,2,E,CS,RS>& m) {
    const E               d  ( det(m) );
    const typename CNT<E>::TInvert ood( typename CNT<E>::StdNumber(1)/d );
    return typename Mat<2,2,E,CS,RS>::TInvert( E( ood*m(1,1)), E(-ood*m(0,1)),
                                               E(-ood*m(1,0)), E( ood*m(0,0)));
}

/// Specialized 3x3 inverse: costs one divide plus 45 flops (for real-valued
/// matrices). No pivoting done here so this may be subject to numerical errors 
/// that Lapack would avoid. Call lapackInverse() instead if you're worried.
/// @see lapackInverse()
template <class E, int CS, int RS> inline
typename Mat<3,3,E,CS,RS>::TInvert inverse(const Mat<3,3,E,CS,RS>& m) {
    // Calculate determinants for each 2x2 submatrix with first row removed.
    // (See the specialized 3x3 determinant routine above.) We're calculating
    // this explicitly here because we can re-use the intermediate terms.
    const E d00(m(1,1)*m(2,2)-m(1,2)*m(2,1)),
            d01(m(1,0)*m(2,2)-m(1,2)*m(2,0)),
            d02(m(1,0)*m(2,1)-m(1,1)*m(2,0));

    // This is the 3x3 determinant and its inverse.
    const E d  (m(0,0)*d00 - m(0,1)*d01 + m(0,2)*d02);
    const typename CNT<E>::TInvert 
            ood(typename CNT<E>::StdNumber(1)/d);

    // The other six 2x2 determinants we can't re-use, but we can still
    // avoid some copying by calculating them explicitly here.
    const E d10(m(0,1)*m(2,2)-m(0,2)*m(2,1)),
            d11(m(0,0)*m(2,2)-m(0,2)*m(2,0)),
            d12(m(0,0)*m(2,1)-m(0,1)*m(2,0)),
            d20(m(0,1)*m(1,2)-m(0,2)*m(1,1)),
            d21(m(0,0)*m(1,2)-m(0,2)*m(1,0)),
            d22(m(0,0)*m(1,1)-m(0,1)*m(1,0));

    return typename Mat<3,3,E,CS,RS>::TInvert
       ( E( ood*d00), E(-ood*d10), E( ood*d20),
         E(-ood*d01), E( ood*d11), E(-ood*d21),
         E( ood*d02), E(-ood*d12), E( ood*d22) ); 
}

/// For any matrix larger than 3x3, we just punt to the Lapack implementation.
/// @see lapackInverse()
template <int M, class E, int CS, int RS> inline
typename Mat<M,M,E,CS,RS>::TInvert inverse(const Mat<M,M,E,CS,RS>& m) {
    return lapackInverse(m);
}

// Implement the Mat<>::invert() method using the above specialized 
// inverse functions. This will only compile if M==N.
template <int M, int N, class ELT, int CS, int RS> inline
typename Mat<M,N,ELT,CS,RS>::TInvert 
Mat<M,N,ELT,CS,RS>::invert() const {
    return SimTK::inverse(*this);
}

} //namespace SimTK


#endif //SimTK_SIMMATRIX_SMALLMATRIX_MIXED_H_
