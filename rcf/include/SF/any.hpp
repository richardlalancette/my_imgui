
//******************************************************************************
// RCF - Remote Call Framework
//
// Copyright (c) 2005 - 2013, Delta V Software. All rights reserved.
// http://www.deltavsoft.com
//
// RCF is distributed under dual licenses - closed source or GPL.
// Consult your particular license for conditions of use.
//
// If you have not purchased a commercial license, you are using RCF 
// under GPL terms.
//
// Version: 2.0
// Contact: support <at> deltavsoft.com 
//
//******************************************************************************

#ifndef INCLUDE_SF_ANY_HPP
#define INCLUDE_SF_ANY_HPP

#include <boost/any.hpp>

#include <RCF/Export.hpp>

namespace SF {

    class Archive;

    RCF_EXPORT void serialize(SF::Archive &ar, boost::any &a);

} // namespace SF

#endif // ! INCLUDE_SF_ANY_HPP
