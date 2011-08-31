
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2011, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful, but   *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *  General Public License for more details.                              *
 *                                                                        *
 *  You should have received a copy of the GNU General Public             *
 *  License along with this program; if not, write to the Free            *
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,       *
 *  MA 02110-1301, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

#include "nQuaternion25.h"
#include "nfield25.h"
#include "maths/nrational.h"
#include <iostream>

namespace regina {

/**
 * Assignment of quaternions.
 */
NQuaternion25 NQuaternion25::operator = (const NQuaternion25& other)
 {
 Rc = other.Rc; Ic = other.Ic; Jc = other.Jc; Kc = other.Kc;
 return (*this);
 }

/**
 * Addition of quaternions.
 */
NQuaternion25 NQuaternion25::operator + (const NQuaternion25& other) const
{
 NQuaternion25 temp(Rc + other.Rc, Ic + other.Ic, Jc + other.Jc, Kc + other.Kc);
 return temp;
}

/**
 * Multiplication of quaternions.
 */
NQuaternion25 NQuaternion25::operator * (const NQuaternion25& other) const
{
  NField25 tRc, tIc, tJc, tKc;

  tRc = Rc*other.Rc - Ic*other.Ic - Jc*other.Jc - Kc*other.Kc;
  tIc = Rc*other.Ic + Ic*other.Rc + Jc*other.Kc - Kc*other.Jc;
  tJc = Rc*other.Jc - Ic*other.Kc + Jc*other.Rc + Kc*other.Ic;
  tKc = Rc*other.Kc + Ic*other.Jc - Jc*other.Ic + Kc*other.Rc;

  return NQuaternion25(tRc,tIc,tJc,tKc); 
}

/**
 * Vector space op
 */
NQuaternion25 NQuaternion25::operator * (const NField25& other) const
{
	NQuaternion25 retval( *this );
	retval.Rc *= other;
	retval.Ic *= other;
	retval.Jc *= other;
	retval.Kc *= other;
	return retval;
}

/**
 * Multiplicative inverse
 */
NQuaternion25 NQuaternion25::inverse() const
{
  NQuaternion25 temp( *this );  
  temp.Ic.negate();
  temp.Jc.negate();
  temp.Kc.negate();
  temp =  temp * (normsquared().inverse());
  return temp;
}

void NQuaternion25::writeTextShort(std::ostream& out) const 
{
 // eventually should write it in a normalized format? 
 // or maybe only do that for TeX output?

 bool ws=false; // written something	
 if ( (Ic != NRational::zero) || (Jc != NRational::zero) || (Kc != NRational::zero) ) 
  {
  if (Rc != NRational::zero)      {      Rc.writeTextShort(out); ws=true; }
  if (Ic != NRational::zero) 
     { 
      if ( (ws) && (Ic.requires_padding()) ) out<<"+";
       if (Ic.nnzt()>1) out<<"(";
       Ic.writeTextShort(out); 
       if (Ic.nnzt()>1) out<<")";
       out<<"i"; ws=true; 
      }
  if (Jc != NRational::zero) 
      { 
        if ( (ws) && (Jc.requires_padding()) ) out<<"+"; 
        if (Jc.nnzt()>1) out<<"(";
        Jc.writeTextShort(out);       
        if (Jc.nnzt()>1) out<<")";
        out<<"j"; ws=true; 
      }
  if (Kc != NRational::zero) 
      { 
       if ( (ws) && (Kc.requires_padding()) ) out<<"+"; 
       if (Kc.nnzt()>1) out<<"(";
        Kc.writeTextShort(out); 
       if (Kc.nnzt()>1) out<<")";
        out<<"k"; 
      }
  }
 else
  {
  Rc.writeTextShort(out);  
  }
}

/**
 * Writes TeX output
 */
void NQuaternion25::writeTeX(std::ostream& out) const
{
 bool ws=false; // written something	
 if ( (Ic != NRational::zero) || (Jc != NRational::zero) || (Kc != NRational::zero) ) 
  {
  if (Rc != NRational::zero)      {      Rc.writeTeX(out); ws=true; }
  if (Ic != NRational::zero) 
      { 
       if ( (ws) && (Ic.requires_padding()) ) out<<"+";
       if (Ic.nnzt()>1) out<<"(";
       if ((Ic.nnzt()==1) && !Ic.requires_padding()) { out<<"-"; (-Ic).writeTeX(out); } else Ic.writeTeX(out); 
       if (Ic.nnzt()>1) out<<")";
       out<<"i"; ws=true; 
      }
  if (Jc != NRational::zero) 
      { 
       if ( (ws) && (Jc.requires_padding()) ) out<<"+"; 
       if (Jc.nnzt()>1) out<<"(";
       if ((Jc.nnzt()==1) && !Jc.requires_padding()) { out<<"-"; (-Jc).writeTeX(out); } else Jc.writeTeX(out);       
       if (Jc.nnzt()>1) out<<")";
        out<<"j"; ws=true; 
      }
  if (Kc != NRational::zero) 
      { 
       if ( (ws) && (Kc.requires_padding()) ) out<<"+"; 
       if (Kc.nnzt()>1) out<<"(";
       if ((Kc.nnzt()==1) && !Kc.requires_padding()) { out<<"-"; (-Kc).writeTeX(out); } else  Kc.writeTeX(out); 
       if (Kc.nnzt()>1) out<<")";
        out<<"k"; 
      }
  }
 else
  {
  Rc.writeTeX(out);  
  }
}

/**
 * determines if is equal to zero
 */
bool NQuaternion25::iszero() const
{
 bool retval;
 if ( (Rc==NRational::zero) && (Ic==NRational::zero) && (Jc==NRational::zero) && (Kc==NRational::zero) ) retval=true;
	else retval=false;
 return retval;
}

/**
 * determines if is equal to an NRational
 */
bool NQuaternion25::operator == ( const NRational& other ) const
{
 bool retval;
 if ( (Rc==other) && (Ic==NRational::zero) && (Jc==NRational::zero) && (Kc==NRational::zero) ) retval=true;
	else retval=false;
 return retval;
}

std::ostream& operator << (std::ostream& out, const NQuaternion25& dat)
{
	dat.writeTextShort(out);
	return out;
}


} // namespace regina

