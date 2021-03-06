// Copyright (C) 2015-2017 Leonardo Silvestri
//
// This file is part of ztsdb.
//
// ztsdb is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ztsdb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ztsdb.  If not, see <http://www.gnu.org/licenses/>.

// For the algo's we use a modified version of Howard Hinnant's code
// in "date.h" (http://howardhinnant.github.io/date_v2.html with the
// algos further explained here:
// http://howardhinnant.github.io/date_algorithms.html). The license
// for this modified code is:

// The MIT License (MIT)
// 
// Copyright (c) 2015 Howard Hinnant
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <ztsdb/globals.hpp>
#include "zconversion.hpp"


static Rcpp::List convertDimnames(const std::vector<std::unique_ptr<arr::Dname>>& dnames) {
  Rcpp::List l;
  for (auto& n : dnames) {
    Rcpp::CharacterVector cv;
    for (auto& e : n->names) {
      cv.push_back(e);
    }
    l.push_back(cv);
  }
  return l;
}


template <int RTYPE, typename T>
static Rcpp::Vector<RTYPE> convertArray(const arr::Array<T>& a) {
  // Note: a first version of these functions used
  // 'Rcpp::Vector::push_back', but that proved to be incredibly slow,
  // so instead we allocate the vector upfront and then subassign.

  Rcpp::Vector<RTYPE> v(a.size()); // unfortunately we have initialization here...
                                   // we would be faster using directly the SEXP
                                   // allocate/populate SEXP, then construct v with it
  uint64_t offset = 0;
  for (const auto& av : a.v) {
    for (arr::idx_type j=0; j<a.nrows(); ++j) {
      v[offset + j] = (*av)[j];
    }
    offset += a.nrows();
  }
  if (a.getdim().size() > 1) {
    v.attr("dim") = Rcpp::IntegerVector(a.getdim().begin(), a.getdim().end());
  }
  if (a.hasNames()) {
    if (a.getdim().size() == 1) {
      v.attr("names") = convertDimnames(a.names)[0];
    }
    else {
      v.attr("dimnames") = convertDimnames(a.names);
    }
  }
  return v;
}


static Rcpp::NumericVector convertDtimeArray(const arr::Array<Global::dtime>& a) {
  Rcpp::NumericVector v(a.size());
  uint64_t offset = 0;
  for (const auto& av : a.v) {
    for (arr::idx_type j=0; j<a.nrows(); ++j) {
      auto dt = (*av)[j];
      double d = dt.time_since_epoch().count() / 1e9;
      auto r = dt.time_since_epoch().count() % 1000; // verify that we are not rounding
      Rcpp::Function warning("warning");
      if (r) {
        warning("time precision loss");
      }
      v[offset + j] = d;
    }
    offset += a.nrows();
  }
  v.attr("class") = Rcpp::CharacterVector{"POSIXct", "POSIXt"};
  if (a.getdim().size() > 1) {
    v.attr("dim") = Rcpp::IntegerVector(a.getdim().begin(), a.getdim().end());
  }
  if (a.hasNames()) {
    if (a.getdim().size() == 1) {
      v.attr("names") = convertDimnames(a.names)[0];
    }
    else {
      v.attr("dimnames") = convertDimnames(a.names);
    }
  }
  return v;
}


static Rcpp::NumericVector convertDtimeVector(const Vector<Global::dtime>& a) {
  Rcpp::NumericVector v(a.size());
  for (arr::idx_type j=0; j<a.size(); ++j) {
    auto dt = a[j];
    double d = dt.time_since_epoch().count() / 1e9;
    auto r = dt.time_since_epoch().count() % 1000; // verify that we are not rounding
    Rcpp::Function warning("warning");
    if (r) {
      warning("time precision loss");
    }
    v[j] = d;
  }
  v.attr("tclass") = Rcpp::CharacterVector{"POSIXct", "POSIXt"};
  return v;
}


static Rcpp::Vector<STRSXP> convertStringArray(const arr::Array<arr::zstring>& a) {
  Rcpp::Vector<STRSXP> v(a.size()); // unfortunately we have initialization here...
                                    // we would be faster using directly the SEXP
  uint64_t offset = 0;
  for (const auto& av : a.v) {
    for (arr::idx_type j=0; j<a.nrows(); ++j) {
      v[offset + j] = std::string((*av)[j]);
    }
    offset += a.nrows();
  }
  if (a.getdim().size() > 1) {
    v.attr("dim") = Rcpp::IntegerVector(a.getdim().begin(), a.getdim().end());
  }
  if (a.hasNames()) {
    if (a.getdim().size() == 1) {
      v.attr("names") = convertDimnames(a.names)[0];
    }
    else {
      v.attr("dimnames") = convertDimnames(a.names);
    }
  }
  return v;
}


union int64_double {
  int64_t i;
  double d;
};


static Rcpp::NumericVector convertDurationArray(const arr::Array<Global::duration>& a) {
  Rcpp::NumericVector v;
  for (arr::idx_type i=0; i<a.size(); ++i) {
    int64_double id;
    id.i = a[i].count();
    v.push_back(id.d);
  }
  v.attr("class") = Rcpp::CharacterVector{"integer64"};
  if (a.getdim().size() > 1) {
    v.attr("dim") = Rcpp::IntegerVector(a.getdim().begin(), a.getdim().end());
  }
  if (a.hasNames()) {
    if (a.getdim().size() == 1) {
      v.attr("names") = convertDimnames(a.names)[0];
    }
    else {
      v.attr("dimnames") = convertDimnames(a.names);
    }
  }
  return v;
}


static Rcpp::NumericVector convertZts(const arr::zts& z) {
  // for this function, we just set following the output on the R
  // command line of 'attributes(xts_object)'.

  // we see that an 'xts' is a 'NumericVector' with an index as
  // attribute 'index'; the following takes care of attributes {dim, dimnames}:
  Rcpp::NumericVector xts = convertArray<REALSXP, double>(z.getArray());

  // now set the index:
  xts.attr("index") = convertDtimeVector(z.getIndexPtr()->getcol(0));

  // and the remaining 
  xts.attr("tzone") = Rcpp::CharacterVector{""};
  xts.attr("tclass") = Rcpp::CharacterVector{"POSIXct", "POSIXt"};
  xts.attr(".indexCLASS") = Rcpp::CharacterVector{"POSIXct", "POSIXt"};
  xts.attr(".indexTZ") = Rcpp::CharacterVector{""};
  xts.attr("descr") = Rcpp::CharacterVector{""};
  xts.attr("class") = Rcpp::CharacterVector{"xts", "zoo"};
  return xts;
}


static Rcpp::List convertList(const arr::Array<val::Value>& a) {
  Rcpp::List l(a.size());
  for (size_t j=0; j<a.size(); ++j) {
    l[j] = valueToSEXP(a[j]);
  }
  if (a.hasNames()) {
    auto names = Rcpp::CharacterVector(a.size());
    for (size_t j=0; j<a.size(); ++j) {
      names[j] = a.getNames(0)[j];
    }
    l.names() = names;
  }
  return l;
}


SEXP valueToSEXP(const val::Value& v) {
  switch (v.which()) {
  case val::vt_zts: {
    const auto& ts = get<val::SpZts>(v);
    Rcpp::NumericVector v = convertZts(*ts);
    return Rcpp::wrap(v);
  }
  case val::vt_double: {
    const auto& a = get<val::SpVAD>(v);
    Rcpp::NumericVector v = convertArray<REALSXP, double>(*a);
    return Rcpp::wrap(v);
  }
  case val::vt_bool: {
    const auto& a = get<val::SpVAB>(v);
    Rcpp::LogicalVector v = convertArray<LGLSXP, bool>(*a);
    return Rcpp::wrap(v);
  }
  case val::vt_time: {
    const auto& a = get<val::SpVADT>(v);
    Rcpp::NumericVector v = convertDtimeArray(*a);
    return Rcpp::wrap(v);
  }
  case val::vt_duration: {
    const auto& a = get<val::SpVADUR>(v);
    Rcpp::NumericVector v = convertDurationArray(*a);
    return Rcpp::wrap(v);
  }
  case val::vt_string: {
    const auto& a = get<val::SpVAS>(v);
    Rcpp::CharacterVector v = convertStringArray(*a);
    return Rcpp::wrap(v);
  }
  case val::vt_list: {
    const auto& a = get<val::SpVList>(v);
    Rcpp::List l = convertList(a->a);
    return Rcpp::wrap(l);
  }
  case val::vt_error: {
    const auto& e = get<val::VError>(v);
    throw std::range_error(e.what);
  }
  default:
    std::cout << "return value: " << v.which() << std::endl;
    throw std::range_error("unkown return value");
  }
}


