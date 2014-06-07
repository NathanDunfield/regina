/*
 * Copyright (c) 2013 Masahide Kashiwagi (kashi@waseda.jp)
 */

#ifndef KRAW_APPROX_HPP
#define KRAW_APPROX_HPP

// 近似解を元にしたKrawczyk法
// 事前に軽くNewton法で精度改善を試みることも出来る。

#include <limits>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <snappea/hikmot/kv/interval.hpp>
#include <snappea/hikmot/kv/rdouble.hpp>
#include <snappea/hikmot/kv/interval-vector.hpp>
#include <snappea/hikmot/kv/autodif.hpp>
#include <snappea/hikmot/kv/matrix-inversion.hpp>
#include <snappea/hikmot/kv/make-candidate.hpp>

namespace regina { namespace kv {

namespace ub = boost::numeric::ublas;

template <class T, class F>
bool
krawczyk_approx(F f, ub::vector<T>& c, ub::vector< interval<T> >& result, ub::vector< interval<T> >& I, ub::matrix<T>& R, int newton_max = 2, int verbose = 1)
{
	int s = c.size();

	//ub::vector< interval<T> > I, fc, fi, Rfc, C, K;
	ub::vector< interval<T> > fc, fi, Rfc, C, K;
	ub::matrix< interval<T> > fdc, fdi, M;
	//ub::matrix<T> R;
	
	std::cout.precision(17);

	int i;
	bool r;
	ub::vector<T> newton_step;
	T tmp, tmp2;

	C = c;

	for (i=0; i<newton_max; i++) {
		// newton法をかける
		// fが単なるTを受け付けない場合も考え、
		// fにはintervalを食わせる。

		try {
			autodif< interval<T> >::split(f(autodif< interval<T> >::init(C)), fc, fdc);
		}
		catch (std::range_error& e) {
			return false;
		}
		r = invert(mid(fdc), R);
		//std::cout << mid(fdc) << "\n";
		if (!r) {
			return false;
		}
		C = C - prod(R, fc);
		C = mid(C);
		if (verbose >= 1) {
			std::cout << "newton" << i << ": " << C << "\n";
		}
	}

	try {
		autodif< interval<T> >::split(f(autodif< interval<T> >::init(C)), fc, fdc);
	}
	catch (std::range_error& e) {
		return false;
	}
	r = invert(mid(fdc), R);
		
	if (!r) return false;
	Rfc = prod(R, fc);

	newton_step.resize(s);
	for (i=0; i<s; i++) {
		newton_step(i) = norm(Rfc(i));
	}

	make_candidate(newton_step);

	I = C;
	for (i=0; i<s; i++) {
		tmp = std::numeric_limits<T>::epsilon() * norm(I(i)) * s * 2.;
		tmp2 = std::numeric_limits<T>::min() * s * 2.;
		if (newton_step(i) < tmp) newton_step(i) = tmp;
		if (newton_step(i) < tmp2) newton_step(i) = tmp2;
		I(i) += newton_step(i) * interval<T>(-1., 1.);
	}

	if (verbose >= 1) {
		std::cout << "I: " << I << "\n";
	}

	try {
		autodif< interval<T> >::split(f(autodif< interval<T> >::init(I)), fi, fdi);
	}
	catch (std::range_error& e) {
		return false;
	}

	// M = ub::identity_matrix< interval<T> >(s) - prod(R, fdi);
	M = ub::identity_matrix< interval<T> >(s);
	M -= prod(R, fdi);

	K = C - Rfc +  prod(M, I - C);

	if (verbose >= 1) {
		std::cout << "K: " << K << "\n";
	}

	if (proper_subset(K, I)) {
		result = K;
		return true;
	} else {
		return false;
	}
}

} } // namespace regina::kv

#endif // KRAW_APPROX_HPP