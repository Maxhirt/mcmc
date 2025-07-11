/*################################################################################
  ##
  ##   Copyright (C) 2016-2023 Keith O'Hara
  ##
  ##   This file is part of the BaseMatrixOps C++ library.
  ##
  ##   Licensed under the Apache License, Version 2.0 (the "License");
  ##   you may not use this file except in compliance with the License.
  ##   You may obtain a copy of the License at
  ##
  ##       http://www.apache.org/licenses/LICENSE-2.0
  ##
  ##   Unless required by applicable law or agreed to in writing, software
  ##   distributed under the License is distributed on an "AS IS" BASIS,
  ##   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ##   See the License for the specific language governing permissions and
  ##   limitations under the License.
  ##
  ################################################################################*/

#ifndef BMO_MATOPS_RANDU_VEC

//

#ifdef BMO_ENABLE_ARMA_WRAPPERS
// acces columns j through k
#define BMO_MATOPS_RANDU_VEC(j) arma::randu(j, 1)
#define BMO_MATOPS_RANDU_ROWVEC(j) arma::randu(1, j)
#define BMO_MATOPS_RANDU_MAT(j, k) arma::randu(j, k)
#endif

#ifdef BMO_ENABLE_EIGEN_WRAPPERS
// acces columns j through k
#define BMO_MATOPS_RANDU_VEC(j)                                                \
  (bmo::ColVec_t::Random(j).array() * 0.5 + 0.5).matrix()
#define BMO_MATOPS_RANDU_ROWVEC(j)                                             \
  (bmo::ColVec_t::Random(j).array() * 0.5 + 0.5).matrix().transpose()
#define BMO_MATOPS_RANDU_MAT(j, k)                                             \
  (bmo::Mat_t::Random(j, k).array() * 0.5 + 0.5).matrix()
#endif

//

#endif
