// ======================================================================
//
// Copyright (c) 2000 The CGAL Consortium
//
// This software and related documentation is part of an INTERNAL release
// of the Computational Geometry Algorithms Library (CGAL). It is not
// intended for general use.
//
// ----------------------------------------------------------------------
//
// release       :
// release_date  :
//
// file          : include/CGAL/Cartesian/vector_constructions_3.h
// revision      : $Revision$
// revision_date : $Date$
// author(s)     : Herve Bronnimann
// coordinator   : INRIA Sophia-Antipolis (Mariette.Yvinec@sophia.inria.fr)
//
// ======================================================================

#ifndef CGAL_CARTESIAN_VECTOR_CONSTRUCTIONS_3_H
#define CGAL_CARTESIAN_VECTOR_CONSTRUCTIONS_3_H

#include <CGAL/Cartesian/Vector_3.h>
#include <CGAL/constructions/kernel_ftC3.h>

CGAL_BEGIN_NAMESPACE

template < class K >
inline
typename K::Vector_3
cross_product(const VectorC3<K> &v, const VectorC3<K> &w)
{
  return K().construct_cross_product_vector_3_object()(v, w);
}

CGAL_END_NAMESPACE

#endif // CGAL_CARTESIAN_VECTOR_CONSTRUCTIONS_3_H
