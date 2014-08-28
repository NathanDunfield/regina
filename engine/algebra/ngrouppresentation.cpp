
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2013, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  As an exception, when this program is distributed through (i) the     *
 *  App Store by Apple Inc.; (ii) the Mac App Store by Apple Inc.; or     *
 *  (iii) Google Play by Google Inc., then that store may impose any      *
 *  digital rights management, device limits and/or redistribution        *
 *  restrictions that are required by its terms of service.               *
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

#include <cctype>
#include <iterator>
#include <list>
#include <map>
#include <sstream>
#include <algorithm>

#include "algebra/ngrouppresentation.h"
#include "algebra/nhomgrouppresentation.h"
#include "algebra/nmarkedabeliangroup.h"
#include "algebra/nabeliangroup.h"
#include "maths/numbertheory.h"
#include "utilities/boostutils.h"
#include "utilities/stlutils.h"
#include "maths/matrixops.h"

namespace regina {

typedef std::list<NGroupExpressionTerm>::iterator TermIterator;
typedef std::list<NGroupExpressionTerm>::const_iterator TermIteratorConst;
typedef std::vector<NGroupExpression*>::iterator RelIterator;
typedef std::vector<NGroupExpression*>::const_iterator RelIteratorConst;
typedef std::list<NGroupExpression*>::iterator TmpRelIterator;

std::ostream& operator << (std::ostream& out,
        const NGroupExpressionTerm& term) {
    if (term.exponent == 0)
        out << '1';
    else if (term.exponent == 1)
        out << 'g' << term.generator;
    else
        out << 'g' << term.generator << '^' << term.exponent;
    return out;
}

NGroupExpressionTerm& NGroupExpression::getTerm(unsigned long index) {
    TermIterator pos = terms.begin();
    advance(pos, index);
    return *pos;
}

const NGroupExpressionTerm& NGroupExpression::getTerm(
        unsigned long index) const {
    TermIteratorConst pos = terms.begin();
    advance(pos, index);
    return *pos;
}

NGroupExpression* NGroupExpression::inverse() const {
    NGroupExpression* ans = new NGroupExpression();
    transform(terms.begin(), terms.end(), front_inserter(ans->terms),
        std::mem_fun_ref(&NGroupExpressionTerm::inverse));
    return ans;
}

void NGroupExpression::invert() {
	reverse(terms.begin(), terms.end());
	std::list< NGroupExpressionTerm >::iterator it; 
	for (it = terms.begin(); it != terms.end(); it++)
		(*it).exponent = -(*it).exponent;
}

NGroupExpression* NGroupExpression::power(long exponent) const {
    NGroupExpression* ans = new NGroupExpression();
    if (exponent == 0)
        return ans;

    long i;
    if (exponent > 0)
        for (i = 0; i < exponent; i++)
            ans->terms.insert(ans->terms.end(), terms.begin(), terms.end());
    else
        for (i = 0; i > exponent; i--)
            transform(terms.begin(), terms.end(), front_inserter(ans->terms),
                std::mem_fun_ref(&NGroupExpressionTerm::inverse));
    return ans;
}

bool NGroupExpression::simplify(bool cyclic) {
    bool changed = false;
    TermIterator next, tmpIt;
    for (next = terms.begin(); next != terms.end(); ) {
        // Take a look at merging next forwards.
        if ((*next).exponent == 0) {
            // Zero exponent.
            // Delete this term and step back to the previous term in
            // case we can now merge the previous and next terms.
            next = terms.erase(next);
            if (next != terms.begin())
                next--;
            changed = true;
            continue;
        }

        tmpIt = next;
        tmpIt++;

        // Now tmpIt points to the term after next.
        if (tmpIt == terms.end()) {
            // No term to merge forwards with.
            next++;
        } else if ((*tmpIt) += (*next)) {
            // Successfully merged this with the following term.
            next = terms.erase(next);
            changed = true;
            // Look at this term again to see if it can be merged further.
        } else {
            // Different generators; cannot merge.
            next++;
        }
    }

    if (! cyclic)
        return changed;

    // Now trying merging front and back terms.
    // We shall do this by popping terms off the back and merging them
    // with the front term.
    while (terms.begin() != terms.end() &&
            ++terms.begin() != terms.end()) {
        // Thus terms.size() > 1.  The unusual test above is used to
        // avoid calling terms.size() which takes linear time.
        if (terms.front() += terms.back()) {
            // Merged!
            terms.pop_back();
            changed = true;

            // Did we create an empty term?
            if (terms.front().exponent == 0)
                terms.pop_front();
        } else
            break;
    }

    return changed;
}

bool NGroupExpression::substitute(unsigned long generator,
        const NGroupExpression& expansion, bool cyclic) {
    bool changed = false;
    NGroupExpression* inverse = 0;
    const NGroupExpression* use;
    long exponent, i;
    for (TermIterator current = terms.begin(); current != terms.end(); ) {
        if ((*current).generator != generator)
            current++;
        else {
            exponent = (*current).exponent;
            if (exponent != 0) {
                if (exponent > 0)
                    use = &expansion;
                else {
                    if (inverse == 0)
                        inverse = expansion.inverse();
                    use = inverse;
                    exponent = -exponent;
                }

                // Fill in exponent copies of use.
                //
                // Note that the following insertion will invalidate
                // current if the wrong type of data structure is being used!
                for (i = 0; i < exponent; i++)
                    terms.insert(current, use->terms.begin(), use->terms.end());
            }

            current = terms.erase(current);
            changed = true;
        }
    }
    if (inverse)
        delete inverse;
    if (changed)
        simplify(cyclic);
    return changed;
}

NGroupPresentation::NGroupPresentation(const NGroupPresentation& cloneMe) :
        ShareableObject(), nGenerators(cloneMe.nGenerators) {
    transform(cloneMe.relations.begin(), cloneMe.relations.end(),
        back_inserter(relations), FuncNewCopyPtr<NGroupExpression>());
}

// TODO: To add: platonic groups, octahedral/cubical, dihedral,
//       icosahedral/dodecahedral, tetrahedral and binary versions of them.  
//       Also need to recognise circle bundles over surfaces. 
//       Free products with amalgamation. Currently intelligentSimplify() 
//       isn't smart enough for this. 
std::string NGroupPresentation::recogniseGroup() const {
    std::ostringstream out;

    // Run through cases.
    if (nGenerators == 0)
        { out << 0; return out.str(); }

    // Let's record the abelianisation.
    std::auto_ptr< NAbelianGroup > ab( abelianisation() );

    // abelian test
    if (isAbelian()) { 
        out << ab.get()->str();
        return out.str();
    }

    // not (clearly) abelian.  Check if free.
    if (relations.size() == 0) { 
        out << "Free(" << nGenerators << ")"; 
        return out.str();
    }

    // Check if its an extension over Z. 
    // TODO: eventually look for extensions over at least fg abelian groups. 
    //   ??maybe?? some other finite groups but it's not clear how to look 
    //   for those. 
    if (ab.get()->getRank()==1) {
        NGroupPresentation presCopy( *this );
        std::auto_ptr< NHomGroupPresentation > AUT( 
            presCopy.identifyExtensionOverZ() );
        if (AUT.get() != NULL) {
            // Let's try to identify the fibre. 
            std::string domStr( AUT.get()->getDomain().recogniseGroup() );
            if (domStr.length()>0) {
                out<<"Z~"<<domStr<<" w/monodromy ";
            unsigned long numGen( 
                AUT.get()->getDomain().getNumberOfGenerators() );
            for (unsigned long i=0; i<numGen; i++) {
              if (i!=0) out<<", ";
              if (numGen<27) out<<( (char) (i+97) ); 
              else out<<"g"<<i;
              out<<" --> ";     
              AUT.get()->evaluate(i).writeText(out, (numGen<27) ? true : false);
              }
             return out.str();
            } // domain not recognised, but it is an extension
          // TODO: put in something here for this case. 
        }
    }         

    std::list< NGroupPresentation* > fpDecomp( identifyFreeProduct() );
    if (fpDecomp.size()>1) {
      out<<"FreeProduct( ";
      for (std::list< NGroupPresentation* >::iterator i=fpDecomp.begin(); 
            i!=fpDecomp.end(); i++) {
        std::string facStr( (*i)->recogniseGroup() );
        if (facStr.length()>0) {
         if (i!=fpDecomp.begin()) out<<", ";
         out<<facStr;
         } 
        else { // TODO: put in something here for this case. 
        }
        delete (*i);
     }
     out<<" )";
     return out.str(); 
    }

    // TODO: let's put in the undergraduate test for finiteness, that every
    //  word can be written as a product of generators a^xb^yc^z with xyz
    //  in a finite interval. Look for the relators that would allow for this
    //  kind of conclusion. 

    return std::string(); // returns empty string if not recognised.
}

std::auto_ptr<NAbelianGroup> NGroupPresentation::abelianisation() const
{
// create presentation matrices to pass to NAbelianGroup(M, N)
    NMatrixInt M(1, getNumberOfGenerators() ); // zero matrix
    NMatrixInt N(getNumberOfGenerators(), getNumberOfRelations() );
// run through rels, increment N entries appropriately
    for (unsigned long j=0; j<getNumberOfRelations(); j++) {
        NGroupExpression Rj ( getRelation(j) );
        for (unsigned long i=0; i<Rj.getNumberOfTerms(); i++)
            N.entry( Rj.getGenerator(i), j ) += Rj.getExponent(i);
    }
    return std::auto_ptr<NAbelianGroup>(new NAbelianGroup(M,N));
}

std::auto_ptr<NMarkedAbelianGroup> NGroupPresentation::markedAbelianisation()
const
{
// create presentation matrices to pass to NMarkedAbelianGroup(M, N)
    NMatrixInt M(1, getNumberOfGenerators() ); // zero matrix
    NMatrixInt N(getNumberOfGenerators(), getNumberOfRelations() );
// run through rels, increment N entries appropriately
    for (unsigned long j=0; j<getNumberOfRelations(); j++) {
        NGroupExpression Rj ( getRelation(j) );
        for (unsigned long i=0; i<Rj.getNumberOfTerms(); i++)
            N.entry( Rj.getGenerator(i), j ) += Rj.getExponent(i);
    }
    return std::auto_ptr<NMarkedAbelianGroup>(new NMarkedAbelianGroup(M,N));
}

void NGroupPresentation::dehnAlgorithmSubMetric(
    const NGroupExpression &this_word,
    const NGroupExpression &that_word,
    std::set< NWordSubstitutionData > &sub_list, unsigned long step )
{
    unsigned long this_length ( this_word.wordLength() );
    unsigned long that_length ( that_word.wordLength() );
// generic early exit strategy
    if ( (this_length < 2) || (that_length==0) ) return;
// early exit strategy based on step.
    if ( (step==1) && ((step+1)*this_length < that_length) ) return;
// TODO: should check to whatever extent the above is of much use...

// this -> splayed to this_word, that_word -> reducer
    std::vector< NGroupExpressionTerm > this_word_vec( 0 );
    std::vector< NGroupExpressionTerm > reducer( 0 );
    this_word_vec.reserve( this_length );
    reducer.reserve( that_length );
    std::list<NGroupExpressionTerm>::const_iterator it;
    for (it = this_word.getTerms().begin(); it!=this_word.getTerms().end(); it++) {
        for (unsigned long i=0; i<abs((*it).exponent); i++)
            this_word_vec.push_back( NGroupExpressionTerm( (*it).generator,
                                     ((*it).exponent>0) ? 1 : -1 ) );
    }
    for (it = that_word.getTerms().begin();
            it!=that_word.getTerms().end(); it++) {
        for (unsigned long i=0; i<abs((*it).exponent); i++)
            reducer.push_back( NGroupExpressionTerm( (*it).generator,
                               ((*it).exponent>0) ? 1 : -1 ) );
    }
    std::vector< NGroupExpressionTerm > inv_reducer( that_length );
    for (unsigned long i=0; i<reducer.size(); i++)
        inv_reducer[that_length-(i+1)] = reducer[i].inverse();

// search for cyclic subwords of reducer in this_word_vec...
    for (unsigned long i=0; i<this_length; i++)
        for (unsigned long j=0; j<that_length; j++) {
            unsigned long comp_length = 0;
            while ( (this_word_vec[(i+comp_length) % this_length] ==
                     reducer[(j+comp_length) % that_length]) &&
                    (comp_length < that_length) && (comp_length < this_length) )
                comp_length++;
            NWordSubstitutionData subData;
            subData.invertB=false;
            subData.sub_length=comp_length;
            subData.start_sub_at=i;
            subData.start_from=j;
            if (comp_length == that_length) {
                subData.score = that_length;
                unsigned long a=1;
                while ( (this_word_vec[( (i+this_length)-a )%this_length].inverse()==
                         this_word_vec[( (i+comp_length)+(a-1) )%this_length]) &&
                        (2*a+that_length <= this_length ) ) {
                    a++;
                    subData.score++;
                }
                sub_list.insert(subData);
            } else if ( comp_length > 0 ) {
                subData.score = 2*comp_length - that_length;
                if ( subData.score > -((long int)step) )
                    sub_list.insert(subData);
            }
            // and the corresponding search with the inverse of reducer.
            comp_length = 0;
            while ( (this_word_vec[(i+comp_length) % this_length] ==
                     inv_reducer[(j+comp_length) % that_length]) &&
                    (comp_length < that_length) && (comp_length < this_length) )
                comp_length++;
            subData.invertB=true;
            subData.sub_length=comp_length;
            if (comp_length == that_length) {
                subData.score = that_length;
                unsigned long a=1;
                while ( (this_word_vec[( (i+this_length)-a )%this_length].inverse()==
                         this_word_vec[( (i+comp_length)+(a-1) )%this_length]) &&
                        (2*a+that_length <= this_length ) ) {
                    a++;
                    subData.score++;
                }
                sub_list.insert(subData);
            } else if ( comp_length > 0 ) {
                subData.score = 2*comp_length - that_length;
                if ( subData.score > -((long int)step) )
                    sub_list.insert(subData);
            }
        }
}

/**
 *  This applies a substitution generated by dehnAlgorithmSubMetric.
 */
void NGroupPresentation::applySubstitution( NGroupExpression& this_word,
        const NGroupExpression &that_word,
        const NWordSubstitutionData &sub_data )
{
// okay, so let's do a quick cut-and-replace, reduce the word and hand it back.
    unsigned long this_length ( this_word.wordLength() );
    unsigned long that_length ( that_word.wordLength() );
    std::vector< NGroupExpressionTerm > this_word_vec( 0 );
    std::vector< NGroupExpressionTerm > reducer( 0 ); // we'll splay-out *this and
    // that_word so that it's easier to search for commonalities.
    this_word_vec.reserve( this_length );
    reducer.reserve( that_length );
    std::list<NGroupExpressionTerm>::const_iterator it;
// start the splaying of terms
    for (it = this_word.getTerms().begin(); it!=this_word.getTerms().end(); it++) {
        for (unsigned long i=0; i<abs((*it).exponent); i++)
            this_word_vec.push_back( NGroupExpressionTerm( (*it).generator,
                                     ((*it).exponent>0) ? 1 : -1 ) );
    }
    // and that_word
    for (it = that_word.getTerms().begin(); it!=that_word.getTerms().end(); it++) {
        for (unsigned long i=0; i<abs((*it).exponent); i++)
            reducer.push_back( NGroupExpressionTerm( (*it).generator,
                               ((*it).exponent>0) ? 1 : -1 ) );
    }
// done splaying, produce inv_reducer
    std::vector< NGroupExpressionTerm > inv_reducer( that_length );
    for (unsigned long i=0; i<that_length; i++) inv_reducer[that_length-(i+1)] =
            reducer[i].inverse();
// done with inv_reducer, erase terms
    this_word.getTerms().clear();

// *this word is some conjugate of AB and the relator is some conjugate of AC.
    //  We are performing the substitution
// A=C^{-1}, thus we need to produce the word C^{-1}B. Put in C^{-1} first..
    for (unsigned long i=0; i<(that_length - sub_data.sub_length); i++)
        this_word.getTerms().push_back( sub_data.invertB ?
                                        reducer[(that_length - sub_data.start_from + i) % that_length] :
                                        inv_reducer[(that_length - sub_data.start_from + i) % that_length] );
// iterate through remainder of this_word_vec, starting from
//     sub_data.start_sub_at + sub_length, ie fill in B
    for (unsigned long i=0; i<(this_length - sub_data.sub_length); i++)
        this_word.getTerms().push_back(
            this_word_vec[(sub_data.start_sub_at + sub_data.sub_length + i) %
                          this_length] );
// done
    this_word.simplify();
}


namespace { // anonymous namespace
    bool compare_length( const NGroupExpression* first, 
             const NGroupExpression* second ) 
     {  return ( first->wordLength() < second->wordLength() ); }

    /**
     *  This routine takes a list of words, together with expVec. It's assumed
     * expVec is initialized to be zero and as long as the number of generators 
     * in the group.  What this routine does is, for each generator of the 
     * group, it records the sum of the absolute value of the exponents of that
     * generator in word.  For the i-th generator this number is recorded in
     * expVec[i]. 
     */
    void build_exponent_vec( const std::list< NGroupExpressionTerm > & word, 
                              std::vector<unsigned long> &expVec ) 
    {
     std::list<NGroupExpressionTerm>::const_iterator tit;
     for ( tit = word.begin(); tit != word.end(); tit++) 
         expVec[ (*tit).generator ] += abs( (*tit).exponent );
    }

    /*
     * Commenting out since this is not used, and requires access to a
     * private struct.  If you want to put it back, it should probably
     * be a member function of NWordSubstitutionData. - bab
     *
    // gives a string that describes the substitution
    std::string substitutionString( const NGroupExpression &word, 
                    const NGroupPresentation::NWordSubstitutionData &subData )
    {
     std::string retval;
     // cut subData into bits, assemble what we're cutting 
     //  out and what we're pasting in. 
     unsigned long word_length ( word.wordLength() );
     std::vector< NGroupExpressionTerm > reducer( 0 ); 
     reducer.reserve( word_length );
     std::list<NGroupExpressionTerm>::const_iterator it; 
      // splay word
     for (it = word.getTerms().begin(); it!=word.getTerms().end(); it++)
      { for (unsigned long i=0; i<abs((*it).exponent); i++)
         reducer.push_back( NGroupExpressionTerm( (*it).generator, 
                          ((*it).exponent>0) ? 1 : -1 ) );    }
     // done splaying, produce inv_reducer
     std::vector< NGroupExpressionTerm > inv_reducer( word_length );
     for (unsigned long i=0; i<word_length; i++)
        inv_reducer[word_length-(i+1)] = reducer[i].inverse(); 
     NGroupExpression del_word, rep_word; 
            // produce word to delete, and word to replace with.

     for (unsigned long i=0; i<(word_length - subData.sub_length); i++)
      rep_word.addTermLast( subData.invertB ?     
            reducer[(word_length - subData.start_from + i) % word_length] : 
        inv_reducer[(word_length - subData.start_from + i) % word_length] );
     for (unsigned long i=0; i<subData.sub_length; i++)
      del_word.addTermLast( subData.invertB ? 
            inv_reducer[(subData.start_from + i) % word_length] : 
                reducer[(subData.start_from + i) % word_length] );
     rep_word.simplify(); del_word.simplify();
     retval = del_word.toString()+" -> "+rep_word.toString();
     return retval;
    }
    */
}

void NGroupExpression::addTermsLast(const NGroupExpression& word)
{
    std::list< NGroupExpressionTerm >::const_iterator it;
    for (it = word.terms.begin(); it != word.terms.end(); it++)
        addTermLast( *it );
}

void NGroupExpression::addTermsFirst(const NGroupExpression& word)
{
    // traverse word's terms in reverse order.
    std::list< NGroupExpressionTerm >::const_reverse_iterator it;
    for (it = word.terms.rbegin(); it != word.terms.rend(); it++)
        addTermFirst( *it );
}

/**
 *  Given a word of the form g_i1^j1 g_i2^j2 ... g_in^jn
 * converts the word into g_i2^j2 ... g_in^jn g_i1^j1
 */
void NGroupExpression::cycleRight()
{
    if (terms.size() > 1) {
        NGroupExpressionTerm temp(terms.front());
        terms.pop_front();
        terms.push_back(temp);
    }
}

/**
 *  Given a word of the form g_i1^j1 g_i2^j2 ... g_in^jn
 * converts the word into g_in^jn g_i1^j1 g_i1^j1 ... g_in-1^jn-1
 */
void NGroupExpression::cycleLeft()
{
    if (terms.size() > 1) {
        NGroupExpressionTerm temp(terms.back());
        terms.pop_back();
        terms.push_front(temp);
    }
}

NGroupExpression::NGroupExpression( const std::string &input, bool* valid )
{
    // interpret input as NGroupExpression as one of forms a^7b^-2,
    // a^7B^2, aaaaaaaBB, g0^7g1^-2. Initialize. If valid allocated set to true
    // or false depending on if operation is successful or not.

    // WSNULL - at start of word, nothing has been input.
    // WSVARLET - read a letter, but do not know if we're in an a^5 or g2^-2 sit.
    // WSVARNUM - gk situation read.
    // WSEXP - ^ read
    // WSEXPSIG ^- read
    // WSEXPNUM reading numbers after ^ or ^-
    // WSERR - an error occured.
    enum WORD_STATUS { WSNULL, WSVARLET, WSVARNUM, WSEXP, WSEXPSIG,
                       WSEXPNUM, WSERR
                     };

    // a loop that goes through the entries of input.
    WORD_STATUS WS(WSNULL);
    NGroupExpressionTerm buildTerm;
    for (std::string::const_iterator i=input.begin(); i!=input.end(); i++) {
        // read *i, see what to do next.
        // case 1: it is a letter a..z or A..Z
        if ( ( (int(*i) >= int('a') ) && ( int(*i) <= int('z') ) ) ||
                ( (int(*i) >= int('A') ) && ( int(*i) <= int('Z') ) ) ) {
            if (WS==WSNULL) { // fresh letter
                // build buildTerm.
                if ( (int(*i) >= int('a') ) && ( int(*i) <= int('z') ) ) {
                    buildTerm.generator = (unsigned long)(*i) - (unsigned long)('a');
                    buildTerm.exponent = 1;
                } else if ( (int(*i) >= int('A') ) && ( int(*i) <= int('Z') ) ) {
                    buildTerm.generator = (unsigned long)(*i) - (unsigned long)('A');
                    buildTerm.exponent = -1;
                } else {
                    WS=WSERR;
                    break;
                }
                WS=WSVARLET;
                continue;
            } else if ( (WS==WSVARLET) || (WS==WSVARNUM) || (WS==WSEXPNUM) ) {
                // new letter but previous letter to finish
                terms.push_back(buildTerm);
                if ( (int(*i) >= int('a') ) && ( int(*i) <= int('z') ) ) {
                    buildTerm.generator = (unsigned long)(*i) - (unsigned long)('a');
                    buildTerm.exponent = 1;
                } else if ( (int(*i) >= int('A') ) && ( int(*i) <= int('Z') ) ) {
                    buildTerm.generator = (unsigned long)(*i) - (unsigned long)('A');
                    buildTerm.exponent = -1;
                } else {
                    WS=WSERR;
                    break;
                }
                WS=WSVARLET;
                continue;
            } else {
                // anything else is a mistake.
                WS=WSERR;    // end case 1
                break;
            }
        }

        // case 2: it is a ^, can only occur after a generator
        if ( (*i) == '^' ) {
            if (!( (WS==WSVARLET) || (WS=WSVARNUM) ) ) {
                WS=WSERR;
                break;
            }
            WS=WSEXP;
            continue;
        } // end case 2

        // case 3: it is a -, only valid after ^
        if ( (*i) == '-' ) {
            if (!(WS==WSEXP)) {
                WS=WSERR;
                break;
            }
            buildTerm.exponent = -buildTerm.exponent; // ok with A^-1.
            WS=WSEXPSIG;
            continue;
        } // end case 3

        // case 4: it is a number
        if ( ::isdigit(*i) ) {
            //  subcase (a) this is to build a variable
            // buildTerm.generator == 'g'
            if ( (WS==WSVARLET) && (buildTerm.generator == ('g' - 'a') ) ) {
                buildTerm.generator=(*i - '0');
                WS=WSVARNUM;
                continue;
            } else // we've already started building the variable number
                if ( WS==WSVARNUM ) {
                    buildTerm.generator=10*buildTerm.generator + (*i - '0');
                    continue;
                } else //  subcase (b) this is to build an exponent.
                    if ( (WS==WSEXP) || (WS==WSEXPSIG) ) { // ^num or ^-num
                        if (buildTerm.exponent<0)
                            buildTerm.exponent = - static_cast<long>(*i - '0');
                        else
                            buildTerm.exponent = (*i - '0');
                        WS=WSEXPNUM;
                        continue;
                    } else if (WS==WSEXPNUM) { // blah[num] previously dealt with numbers
                        if (buildTerm.exponent<0)
                            buildTerm.exponent = 10*buildTerm.exponent -
                                                 (*i - '0');
                        else
                            buildTerm.exponent = 10*buildTerm.exponent +
                                                 (*i - '0');
                        continue;
                    } else {
                        WS=WSERR;
                        break;
                    }
        } // end case 4
        // now we've dealt will all important input.  Let's deal with spaces next,
        //  and any other input will fail.
        if ( ::isspace(*i) ) continue;
        else {
            WS=WSERR;
            break;
        }
    } // end i loop
// did we reach the end of input without any errors? if so, append buildTerm
    if (WS!=WSERR) {
        terms.push_back(buildTerm);
        if (valid != NULL) (*valid) = true;
    }
// if not, delete everything in this word
    else {
        if (valid != NULL) (*valid) = false;
        terms.clear();
    }
}

bool NGroupExpression::addStringFirst( const std::string& input)
{
    bool temp;
    NGroupExpression tword( input, &temp );
    if (!temp)
        return false;
    addTermsFirst(tword);
    return true;
}

bool NGroupExpression::addStringLast( const std::string& input)
{
    bool temp;
    NGroupExpression tword( input, &temp );
    if (!temp)
        return false;
    addTermsLast(tword);
    return true;
}

//             **********  NGroupPresentation below **************

NGroupPresentation::NGroupPresentation(unsigned long nGens,  
            std::vector<std::string> &rels)
{
    nGenerators = nGens;
    for (std::vector<std::string>::iterator i=rels.begin();
         i!=rels.end(); i++)
    relations.push_back( new NGroupExpression(*i) );
}

bool NGroupPresentation::simplifyWord( NGroupExpression &input ) const
{
    bool retval( input.simplify(false) );
    if (input.isTrivial()) return retval;

    // now recursively apply relators until no reduction is possible.
    bool continueSimplify(true);
    while (continueSimplify) {
        continueSimplify = false;
        for (unsigned long i=0; i<relations.size(); i++)  {
            std::set< NWordSubstitutionData > sub_list; // highest score is *first*
            dehnAlgorithmSubMetric( input, *relations[i], sub_list );
            if (sub_list.size() != 0) if ( (*sub_list.begin()).score > 0 ) {
                    applySubstitution( input, *relations[i], *sub_list.begin() );
                    if (input.isTrivial()) return true;
                    continueSimplify = true;
                    retval = true;
                }
        }
    }
    return retval;
}

// for now we iterate:
// 1) Small Cancellation theory
// 2) Nielsen moves until no reducing ones available
// 3) Loop back to (1) until nothing happens in either (1) or (2).
// TODO: consider a homological alignment call if the abelianization
//       has rank 1 or any other situation where we know it can be useful.
std::auto_ptr<NHomGroupPresentation>
NGroupPresentation::intelligentSimplifyDetail()
{
    bool doRep(true), bool1(false), bool2(false);
    std::auto_ptr<NHomGroupPresentation> redHom;
    while (doRep) {
        doRep = false;
        bool1 = false;
        bool2 = false;
        std::auto_ptr<NHomGroupPresentation>
        tempHom( smallCancellationDetail() );
        if (tempHom.get()) {
            bool1 = true;
            if (!redHom.get()) redHom.reset( tempHom.release() );
            else redHom.reset( tempHom->composeWith( *redHom.get() ).release() );
        }

        std::auto_ptr<NHomGroupPresentation>
        temp2Hom( intelligentNielsenDetail() );
        if (temp2Hom.get()) {
            bool2 = true;
            if (!redHom.get()) redHom.reset( temp2Hom.release() );
            else redHom.reset( temp2Hom->composeWith( *redHom.get() ).release() );
        }
        if ( bool2 ) doRep = true;
    }

    std::auto_ptr<NHomGroupPresentation> temp3Hom( prettyRewritingDetail() );
    if (temp3Hom.get()) {
        if (!redHom.get()) redHom.reset( temp3Hom.release() );
        else redHom.reset( temp3Hom->composeWith( *redHom.get() ).release() );
    }

    return std::auto_ptr<NHomGroupPresentation>( redHom.release() );
}

bool NGroupPresentation::intelligentSimplify()
{
    return intelligentSimplifyDetail().get();
}

bool NGroupPresentation::smallCancellation()
{
    return smallCancellationDetail().get();
}

std::auto_ptr<NHomGroupPresentation>
NGroupPresentation::smallCancellationDetail()
{
    bool didSomething(false);
// start by taking a copy of *this group, for construction of reductionMap
    NGroupPresentation oldGroup( *this );

    std::list< NGroupExpression* >::iterator it;
// relatorList will be the temporary relator table.
    std::list< NGroupExpression* > relatorList;
    for (unsigned long i=0; i<relations.size(); i++)
        relatorList.push_back( relations[i] );
    relations.clear();
// substitutionTable[i] == 1 means kill gi.
//                      != gi means replace gi by this.
    std::vector< NGroupExpression > substitutionTable( nGenerators );
    for (unsigned long i=0; i<nGenerators; i++)
        substitutionTable[i].addTermFirst( i, 1 );

    bool we_value_iteration(true);
    while (we_value_iteration) {
        we_value_iteration = false;
        // cyclically reduce relators
        for ( it = relatorList.begin(); it != relatorList.end(); it++)
            (*it)->simplify(true);
        relatorList.sort( compare_length ); // (1)

        it = relatorList.begin();  // start (2) deletion of 0-length relators
        while ( it != relatorList.end() )  {
            if ( (*it)->wordLength() == 0 ) {
                delete (*it);
                it = relatorList.erase(it);
            }  else break;
        }

        // start (3) - apply shorter relators to longer.
        for (it = relatorList.begin(); it != relatorList.end(); it++)
            if ( (*it)->wordLength() > 0 ) { // don't bother if this is a trivial word.
                std::list< NGroupExpression* >::iterator tit; // target of it manips.
                tit = it;
                tit++;
                while (tit != relatorList.end()) {
                    // attempt to apply *it to *tit
                    std::set< NWordSubstitutionData > sub_list;
                    dehnAlgorithmSubMetric( **tit, **it, sub_list ); // take first valid sub
                    if (sub_list.size() != 0) if ( (*sub_list.begin()).score > 0 ) {
                            applySubstitution( **tit, **it, *sub_list.begin() );
                            we_value_iteration = true;
                            didSomething = true;
                        }
                    tit++;
                }
            } // end (3) - application of shorter to longer relators.

        // (4) Build and sort a list (by length) of generator-killing relations.
        relatorList.sort( compare_length );
        for (it = relatorList.begin(); it!=relatorList.end(); it++) {
            bool word_length_3_trigger(false);
            unsigned long WL ( (*it)->wordLength() );
            // build a table expressing number of times each generator is used in *it.
            std::vector< unsigned long > genUsage( nGenerators );
            build_exponent_vec( (*it)->getTerms(), genUsage );

            std::list<NGroupExpressionTerm>::iterator tit;
            for (unsigned long i=0; i<genUsage.size(); i++) if (genUsage[i] == 1) {
                    // have we found a substitution for generator i ?
                    if ( ( substitutionTable[i].getNumberOfTerms() == 1 ) &&
                            ( substitutionTable[i].getGenerator(0) == i ) ) {
                        // we have a valid substitution.  Replace all occurances
                        // of generator genUsage[i] with the inverse of the remaining word
                        bool inv(true);
                        bool before_flag(true); // true if we have not yet encountered gen
                        NGroupExpression prefix, complement;
                        for ( tit = (*it)->getTerms().begin();
                                ( tit != (*it)->getTerms().end() ); tit++) {
                            if ( (*tit).generator == i ) {
                                inv = ((*tit).exponent != 1);
                                before_flag=false;
                            } else {
                                if (before_flag) prefix.addTermLast( (*tit) );
                                else complement.addTermLast( (*tit) );
                            }
                        }
                        complement.addTermsLast(prefix);
                        if (!inv) complement.invert();
                        // sub gi --> complement, in both substitutionTable and relatorList
                        for (unsigned long j=0; j<substitutionTable.size(); j++)
                            substitutionTable[j].substitute( i, complement );
                        for (std::list< NGroupExpression* >::iterator pit = relatorList.begin();
                                pit != relatorList.end(); pit++)
                            (*pit)->substitute( i, complement );
                        we_value_iteration = true;
                        didSomething = true;
                        if (WL>3) word_length_3_trigger=true;
                        goto found_a_generator_killer;
                    }
                } // the look through genUsage loop

found_a_generator_killer:
            if (word_length_3_trigger) break;
        } // end (4)
    } // end of main_while_loop (6)

    nGenerators = 0;
    for (unsigned long i=0; i<substitutionTable.size(); i++)
        if ( substitutionTable[i].getNumberOfTerms() == 1 )
            if ( substitutionTable[i].getGenerator(0) == i ) nGenerators++;

// now we can build up a mapping of where the current generators get sent to.
// make it a std::vector.

    std::vector< unsigned long > genReductionMapping( nGenerators );
    unsigned long indx(0);
    for (unsigned long i=0; i<substitutionTable.size(); i++) {
        if ( substitutionTable[i].getNumberOfTerms() == 1 )
            if ( substitutionTable[i].getGenerator(0) == i ) {
                genReductionMapping[ indx ] = i;
                indx++;
            }
    }

// now let's run through relatorList and substitute genReductionMapping[i] -> i
    for (it = relatorList.begin(); it != relatorList.end(); it++)
        for (unsigned long i=0; i<nGenerators; i++) {
            NGroupExpression gi;
            gi.addTermFirst( i, 1 );
            (*it)->substitute( genReductionMapping[i], gi );
        }
// and might as well do substitutionTable, too.
    for (unsigned long j=0; j<substitutionTable.size(); j++)
        for (unsigned long i=0; i<nGenerators; i++) {
            NGroupExpression gi;
            gi.addTermFirst( i, 1 );
            substitutionTable[j].substitute( genReductionMapping[i], gi );
        }
// okay, now let's replace relations with relatorList
    relations.reserve( relatorList.size() );
    for (it = relatorList.begin(); it != relatorList.end(); it++) {
        relations.push_back( (*it) );
    }
// and build the reverse isomorphism from the new group to the old
    std::vector< NGroupExpression > revMap(nGenerators);
    for (unsigned long i=0; i<revMap.size(); i++)
        revMap[i].addTermFirst( genReductionMapping[i], 1 );

    if (didSomething) {
        // now we can initialize reductionMap
        return std::auto_ptr<NHomGroupPresentation>(new NHomGroupPresentation(
                    oldGroup, *this, substitutionTable, revMap));
    } else
        return std::auto_ptr<NHomGroupPresentation>();
}// end smallCancellation()

NGroupPresentation& NGroupPresentation::operator=(
    const NGroupPresentation& copyMe)
{
    nGenerators = copyMe.nGenerators;
    for (unsigned long i=0; i<relations.size(); i++)
        delete relations[i];
    relations.resize(copyMe.relations.size());
    for (unsigned long i=0; i<relations.size(); i++)
        relations[i] = new NGroupExpression( *copyMe.relations[i] );
    return *this;
}

bool NGroupPresentation::intelligentNielsen()
{
    return intelligentSimplifyDetail().get();
}

std::auto_ptr<NHomGroupPresentation>
NGroupPresentation::intelligentNielsenDetail()
{
    if (nGenerators < 2) return std::auto_ptr<NHomGroupPresentation>(NULL);
// let's keep a record of the best possible substitution,
    bool didSomething(true);
    std::auto_ptr<NHomGroupPresentation> retval(NULL);
    while (didSomething) {
        didSomething = false;
        unsigned long bSubi(0), bSubj(0), bSubType(0); // IJ IJi JI or JIi 0,1,2,3
        signed long bSubScore(0);

        for (unsigned long i=0; i<nGenerators; i++)
            for (unsigned long j=0; j<nGenerators; j++) {
                if (i==j) continue;
                signed long scrIJ(0), scrIJi(0), scrJI(0), scrJIi(0); // ongoing score count.
                // run through all the relators.
                for (unsigned long l=0; l<relations.size(); l++) {
                    NGroupExpression* rel(relations[l]);
                    std::list<NGroupExpressionTerm>& terms( rel->getTerms() );
                    // now we run from front to back, cyclically keeping track of the
                    //  previous and next terms respectively.
                    NGroupExpressionTerm prevTerm(terms.back()),
                                         thisTerm(terms.front()), nextTerm;
                    for (std::list<NGroupExpressionTerm>::iterator T=terms.begin();
                            T!=terms.end(); T++) {
                        std::list<NGroupExpressionTerm>::iterator Tn(T);
                        Tn++;
                        if (T!=terms.begin()) {
                            prevTerm = thisTerm;
                            thisTerm=nextTerm;
                        }
                        nextTerm = ( (Tn == terms.end()) ? terms.front() : *Tn );

                        // check IJ  gigj    -> gi  i.e. gi -> gigj^-1 all other gens fixed
                        if ( (thisTerm.generator == i) && (thisTerm.exponent>0) )
                            ( (nextTerm.generator == j) && (nextTerm.exponent>0) ) ?
                            scrIJ += (2-thisTerm.exponent) : scrIJ -= thisTerm.exponent;
                        if ( (thisTerm.generator == i) && (thisTerm.exponent<0) )
                            ( (prevTerm.generator == j) && (prevTerm.exponent<0) ) ?
                            scrIJ += (2+thisTerm.exponent) : scrIJ += thisTerm.exponent;
                        // check IJi gigj^-1 -> gi i.e. gi -> gigj
                        if ( (thisTerm.generator == i) && (thisTerm.exponent>0) )
                            ( (nextTerm.generator == j) && (nextTerm.exponent<0) ) ?
                            scrIJi += (2-thisTerm.exponent) : scrIJi -= thisTerm.exponent;
                        if ( (thisTerm.generator == i) && (thisTerm.exponent<0) )
                            ( (prevTerm.generator == j) && (prevTerm.exponent>0) ) ?
                            scrIJi += (2+thisTerm.exponent) : scrIJi += thisTerm.exponent;
                        // check JI  gjgi    -> gi
                        if ( (thisTerm.generator == i) && (thisTerm.exponent<0) )
                            ( (nextTerm.generator == j) && (nextTerm.exponent<0) ) ?
                            scrJI += (2+thisTerm.exponent) : scrJI += thisTerm.exponent;
                        if ( (thisTerm.generator == i) && (thisTerm.exponent>0) )
                            ( (prevTerm.generator == j) && (prevTerm.exponent>0) ) ?
                            scrJI += (2-thisTerm.exponent) : scrJI -= thisTerm.exponent;
                        // check JIi gj^-1gi -> gi
                        if ( (thisTerm.generator == i) && (thisTerm.exponent<0) )
                            ( (nextTerm.generator == j) && (nextTerm.exponent>0) ) ?
                            scrJIi += (2+thisTerm.exponent) : scrJIi += thisTerm.exponent;
                        if ( (thisTerm.generator == i) && (thisTerm.exponent>0) )
                            ( (prevTerm.generator == j) && (prevTerm.exponent<0) ) ?
                            scrJIi += (2-thisTerm.exponent) : scrJIi -= thisTerm.exponent;
                    }
                } // end of relator loop
                // compare the IJ, IJi, JI, JIi subs to the best known one, if it exists
                if (scrIJ > bSubScore)  {
                    bSubScore = scrIJ;
                    bSubi = i;
                    bSubj = j;
                    bSubType = 0;
                }
                if (scrIJi > bSubScore) {
                    bSubScore = scrIJi;
                    bSubi = i;
                    bSubj = j;
                    bSubType = 1;
                }
                if (scrJI > bSubScore)  {
                    bSubScore = scrJI;
                    bSubi = i;
                    bSubj = j;
                    bSubType = 2;
                }
                if (scrJIi > bSubScore) {
                    bSubScore = scrJIi;
                    bSubi = i;
                    bSubj = j;
                    bSubType = 3;
                }
            }

        if (bSubScore > 0) {
            // perform sub!
            didSomething=true;
            NGroupPresentation oldPres(*this);
            std::vector<NGroupExpression> DomToRan( oldPres.getNumberOfGenerators() );
            std::vector<NGroupExpression> RanToDom( oldPres.getNumberOfGenerators() );
            for (unsigned long i=0; i<DomToRan.size(); i++) {
                DomToRan[i].addTermFirst( NGroupExpressionTerm( i, 1 ) );
                RanToDom[i].addTermFirst( NGroupExpressionTerm( i, 1 ) );
            }
            if (bSubType == 0) {
                nielsenCombine( bSubi, bSubj, 1, true );
                DomToRan[ bSubi ].addTermLast( NGroupExpressionTerm( bSubj, -1 ) );
                RanToDom[ bSubi ].addTermLast( NGroupExpressionTerm( bSubj, 1 ) );
            } else if (bSubType == 1) {
                nielsenCombine( bSubi, bSubj, -1, true );
                DomToRan[ bSubi ].addTermLast( NGroupExpressionTerm( bSubj, 1 ) );
                RanToDom[ bSubi ].addTermLast( NGroupExpressionTerm( bSubj, -1 ) );
            } else if (bSubType == 2) {
                nielsenCombine( bSubi, bSubj, 1, false );
                DomToRan[ bSubi ].addTermFirst( NGroupExpressionTerm( bSubj, -1 ) );
                RanToDom[ bSubi ].addTermFirst( NGroupExpressionTerm( bSubj, 1 ) );
            } else if (bSubType == 3) {
                nielsenCombine( bSubi, bSubj, -1, false );
                DomToRan[ bSubi ].addTermFirst( NGroupExpressionTerm( bSubj, 1 ) );
                RanToDom[ bSubi ].addTermFirst( NGroupExpressionTerm( bSubj, -1 ) );
            }
            NGroupPresentation newPres(*this);
            std::auto_ptr<NHomGroupPresentation> tempHom(new
                    NHomGroupPresentation(oldPres,newPres,DomToRan,RanToDom));
            if (!retval.get()) retval.reset( tempHom.release() );
            else retval.reset( tempHom->composeWith( *retval.get() ).release() );
            // make the dom->ran and ran->dom vectors.
        }
    } // the while loop

    return std::auto_ptr<NHomGroupPresentation>(retval.release());
}

bool NGroupPresentation::homologicalAlignment()
{
    return homologicalAlignmentDetail().get();
}

std::auto_ptr<NHomGroupPresentation>
NGroupPresentation::homologicalAlignmentDetail()
{
    std::auto_ptr<NHomGroupPresentation> retval; // only allocate if appropriate.
// step 1: compute abelianization and how generators map to abelianization.
    std::auto_ptr< NMarkedAbelianGroup > abelianized( markedAbelianisation() );
    NMatrixInt abMat( abelianized->minNumberOfGenerators(),
                      getNumberOfGenerators() );

    for (unsigned long j=0; j<getNumberOfGenerators(); j++) {
        std::vector<NLargeInteger> epsilon( getNumberOfGenerators() );
        epsilon[j] = 1;
        std::vector<NLargeInteger> temp( abelianized->snfRep(epsilon) );
        for (unsigned long i=0; i<abelianized->minNumberOfGenerators(); i++)
            abMat.entry(i,j) = temp[i]; // columns are snfreps of abelianized gens.
    }

    unsigned long abNF( abelianized->getNumberOfInvariantFactors() );
    unsigned long abNG( abelianized->minNumberOfGenerators() );
// step 2: we will mimic the simple smith normal form algorithm algorithm
//         using corresponding moves on the group presentation.
//         first the free generators.
    for (unsigned long i=abNF; i<abNG; i++) {
        // in row i we will eliminate all but one entry using column
        // operations.  Now we need to do a while loop -- find any two non-zero
        // entries in the row, and reduce.  If there's only one non-zero entry,
        // we're done.
        unsigned long j0=0, j1=abMat.columns()-1;
        while (j0 < j1) {
            // if at j0 its zero, inc, if at j1 zero, dec
            if (abMat.entry(i,j0).isZero()) {
                j0++;
                continue;
            }
            if (abMat.entry(i,j1).isZero()) {
                j1--;
                continue;
            }
            // column op!
            bool colFlag( abMat.entry(i,j0).abs() < abMat.entry(i,j1).abs() );
            NLargeInteger q = abMat.entry(i,colFlag ? j1 : j0) /
                              abMat.entry(i,colFlag ? j0 : j1);
            // subtract q times column j0 from column j1
            for (unsigned long r=0; r<abMat.rows(); r++)
                abMat.entry(r,colFlag ? j1 : j0) -= abMat.entry(r,colFlag ? j0 : j1)*q;
            NGroupPresentation oldPres(*this);
            std::vector<NGroupExpression> fVec( nGenerators ), bVec( nGenerators );
            for (unsigned long l=0; l<nGenerators; l++) {
                fVec[l].addTermLast( NGroupExpressionTerm( l, 1 ) );
                bVec[l].addTermLast( NGroupExpressionTerm( l, 1 ) );
                if (l==j1) {
                    fVec[l].addTermLast( NGroupExpressionTerm(
                                             colFlag ? j0 : j1, q.longValue() ) );
                    bVec[l].addTermLast( NGroupExpressionTerm(
                                             colFlag ? j0 : j1, -q.longValue() ) );
                }
            }
            // manufacture the Nielsen automorphism...
            nielsenCombine(colFlag ? j1 : j0, colFlag ? j0 : j1, -q.longValue() );
            std::auto_ptr<NHomGroupPresentation> tempHom(
                new NHomGroupPresentation( oldPres, *this, fVec, bVec ) );
            if (!retval.get()) retval.reset( tempHom.release() );
            else
                retval.reset( tempHom->composeWith( *retval.get() ).release() );
        } // j0==j1 is the column such that entry (i,j1) is +-1.
        nielsenTransposition(i, j1);
        abMat.swapColumns(i, j1);
        // NOTE: the matrix will have the form:
        //       [ * * * ]
        //       [ 0 D 0 ]  At this point, with D a diagonal +-1 matrix.
    }

    for (unsigned long i=0; i<abNF; i++) for (unsigned long j=abNF; j<abNG; j++)
            abMat.entry(i,j) = 0;
// now we're at [ * 0 * ]
//              [ 0 D 0 ]

// step 3: reduce inv fac terms, kill the rest.
    for (unsigned long i=0; i<abNF; i++) {
        // let's start working on entry(i,j0) and (i,j1) with j0<j1 in 0...invFacNum
        unsigned long j0=0, j1=abMat.columns()-1;
        while (j0 < j1) {
            // if at j0 its zero, inc, if at j1 zero, dec
            if ((abMat.entry(i,j0) % abelianized->getInvariantFactor(i)).isZero()) {
                j0++;
                continue;
            }
            if ((abMat.entry(i,j1) % abelianized->getInvariantFactor(i)).isZero()) {
                j1--;
                continue;
            }
            // column op!
            bool colFlag( (abMat.entry(i,j0) % abelianized->getInvariantFactor(i)).abs() <
                          (abMat.entry(i,j1) % abelianized->getInvariantFactor(i)).abs() );
            NLargeInteger q = abMat.entry(i,colFlag ? j1 : j0) /
                              abMat.entry(i,colFlag ? j0 : j1);

            // subtract q times column j0 from column j1
            for (unsigned long r=0; r<abMat.rows(); r++)
                abMat.entry(r,colFlag ? j1 : j0) -= abMat.entry(r,colFlag ? j0 : j1)*q;
            NGroupPresentation oldPres(*this);
            std::vector<NGroupExpression> fVec( nGenerators ), bVec( nGenerators );
            for (unsigned long l=0; l<nGenerators; l++) {
                fVec[l].addTermLast( NGroupExpressionTerm( l, 1 ) );
                bVec[l].addTermLast( NGroupExpressionTerm( l, 1 ) );
                if (l==j1) {
                    fVec[l].addTermLast( NGroupExpressionTerm(
                                             colFlag ? j0 : j1, q.longValue() ) );
                    bVec[l].addTermLast( NGroupExpressionTerm(
                                             colFlag ? j0 : j1, -q.longValue() ) );
                }
            }
            // manufacture the Nielsen automorphism...
            nielsenCombine(colFlag ? j1 : j0, colFlag ? j0 : j1, -q.longValue() );
            std::auto_ptr<NHomGroupPresentation> tempHom(
                new NHomGroupPresentation( oldPres, *this, fVec, bVec ) );
            if (!retval.get()) retval.reset( tempHom.release() );
            else
                retval.reset( tempHom->composeWith( *retval.get() ).release() );
        } // j0==j1 is the column such that entry (i,j1) is +-1.
        if (i!=j1) {
            nielsenTransposition(i, j1);
            abMat.swapColumns(i, j1);
        }
    }
// now we're at [ P 0 0 ]
//              [ 0 D 0 ] so we're essentially done.

// call prettify
    std::auto_ptr<NHomGroupPresentation> tempHom( prettyRewritingDetail() );
    if (!retval.get() && tempHom.get()) retval.reset( tempHom.release() );
    else if (retval.get() && tempHom.get())
        retval.reset( tempHom->composeWith( *retval.get() ).release() );

    return std::auto_ptr<NHomGroupPresentation>( retval.release() );
}

// This algorithm has to be at least moderately sophisticated to ensure it
// recognises that < a, b, a^2, abaB > is abelian.
bool NGroupPresentation::isAbelian() const
{
// The idea will be to take all commutators of the generators, and see if
//  the relators can kill them.
    for (unsigned long i=0; i<nGenerators; i++)
        for (unsigned long j=i+1; j<nGenerators; j++) {
            // let's see if we can recursively apply the relations to
            // [gi,gj] in order to kill it.
            NGroupExpression COM; // commutator [gi,gj]
            COM.addTermLast( i, 1 );
            COM.addTermLast( j, 1 );
            COM.addTermLast( i, -1 );
            COM.addTermLast( j, -1 );
            simplifyWord( COM );
            if (!COM.isTrivial()) return false;
        }
    return true;
}

/**
 * This routine takes two words as input, *this and other, and
 * determines whether or not one can re-label the generators in
 * this word to get the other word. If so, it returns a non-empty
 * list of such re-labellings. Re-labellings are partially-defined
 * permutations (with possible inversions if cyclic=true) on the
 * generator set.
 *
 * @param other is what the return permutation turn
 * @param cyclic, if false we get a list of exact relabellings from
 *  *this to other.  If true, it can be up to cyclic permutation and
 *  inversion. If cyclic is true, the routine demands both words
 *  are cyclically-reduced.
 */
std::list< std::map< unsigned long, NGroupExpressionTerm > >
NGroupExpression::relabellingsThisToOther( const NGroupExpression &other,
        bool cyclic ) const
{
    // we'll handle the cyclic==true case as a repeated cyclic==false call.
    if (cyclic) {
        std::list< std::map< unsigned long, NGroupExpressionTerm > > retval;
        if (getNumberOfTerms() != other.getNumberOfTerms())
            return retval;
        NGroupExpression tempW( *this );
        for (unsigned long i=0; i<tempW.getNumberOfTerms(); i++) {
            std::list< std::map< unsigned long, NGroupExpressionTerm > > tempL(
                tempW.relabellingsThisToOther( other, false ) );
            // append to retval
            for (std::list< std::map< unsigned long, NGroupExpressionTerm > >::iterator
                    j=tempL.begin(); j!=tempL.end(); j++)
                retval.push_back( *j );
            tempW.cycleRight();
        }
        tempW.invert();
        for (unsigned long i=0; i<tempW.getNumberOfTerms(); i++) {
            std::list< std::map< unsigned long, NGroupExpressionTerm > > tempL(
                tempW.relabellingsThisToOther( other, false ) );
            // append to retval
            for (std::list< std::map< unsigned long, NGroupExpressionTerm > >::iterator
                    j=tempL.begin(); j!=tempL.end(); j++)
                retval.push_back( *j );
            tempW.cycleRight();
        }
        return retval;
        // TODO: consider removing duplicates at this stage.  Or make the return
        //  value a set.
    }

// cyclic==false
    std::map< unsigned long, NGroupExpressionTerm > tempMap;
    std::list<NGroupExpressionTerm>::const_iterator j=other.getTerms().begin();
    std::list<NGroupExpressionTerm>::const_iterator i=getTerms().begin();
    for ( ; ( (i!=getTerms().end()) && (j!=other.getTerms().end()) ); i++, j++)
        if (abs(i->exponent) == abs(j->exponent)) {
            // matching exponents, so check if generators have been used yet.
            std::map< unsigned long, NGroupExpressionTerm >::iterator k;
            k=tempMap.find( i->generator );
            NGroupExpressionTerm MAPTO( j->generator,
                                        (i->exponent == j->exponent) ? 1 : -1 );
            if (k!=tempMap.end()) { // previously defined, check consistency
                if (!(k->second == MAPTO)) // contradicting definition
                    return std::list< std::map< unsigned long, NGroupExpressionTerm > >();
            } else tempMap[i->generator] = MAPTO;
        }
// check if words had different number of terms
    if ( (i!=getTerms().end()) || (j!=other.getTerms().end()) )
        return std::list< std::map< unsigned long, NGroupExpressionTerm > >();

// okay, we have something.
    std::list< std::map< unsigned long, NGroupExpressionTerm > > retval;
    retval.push_back( tempMap );
    return retval;
}

std::list< NGroupPresentation* >
NGroupPresentation::identifyFreeProduct() const
{
// let's create a list of generators not used in the relators, then
// generators that appear in a common generator, or recursively related
    std::set< unsigned long > unRelated;
    for (unsigned long i=0; i<getNumberOfGenerators(); i++)
        unRelated.insert(i);
    std::list< std::set< unsigned long > > equivRel;
// determine which generators are used in the relators, to initialize
// equivRel.
    {
        // forced scope
        std::set< unsigned long > usedRels;
        for (unsigned long i=0; i<relations.size(); i++)
            for (std::list<NGroupExpressionTerm>::const_iterator
                    T=relations[i]->getTerms().begin();
                    T!=relations[i]->getTerms().end(); T++)
                usedRels.insert( T->generator );
        for (std::set< unsigned long >::iterator i=usedRels.begin();
                i!=usedRels.end(); i++) {
            std::set< unsigned long > singleton;
            singleton.insert( *i );
            unRelated.erase( *i );
            equivRel.push_back(singleton);
        }
    } // end forced scope
// now we grow the equivalence relation.

    for (unsigned long i=0; i<relations.size(); i++) {
        std::set< unsigned long > relSet; // related by relations[i]
        if ( (unRelated.size()==0) && (equivRel.size()==1) ) break;
        for (std::list<NGroupExpressionTerm>::const_iterator
                T=relations[i]->getTerms().begin();
                T!=relations[i]->getTerms().end(); T++)
            relSet.insert( T->generator );
        if (relSet.size() < 2) continue; // in case of empty word
        for (std::set< unsigned long >::iterator I=relSet.begin(); I!=relSet.end();
                I++)
            for (std::set< unsigned long >::iterator J=I; J!=relSet.end(); J++) {
                if (I==J) continue;
                std::list< std::set< unsigned long > >::iterator SI;
                std::list< std::set< unsigned long > >::iterator SJ;
                for (SI=equivRel.begin(); SI!=equivRel.end(); SI++)
                    if (SI->find( *I )!=SI->end()) break;
                for (SJ=equivRel.begin(); SJ!=equivRel.end(); SJ++)
                    if (SJ->find( *J )!=SJ->end()) break;
                if (SI==SJ) continue; // so we merge the sets SI and SJ are pointing to.
                SI->insert( SJ->begin(), SJ->end() );
                equivRel.erase( SJ );
            } // I J loop
    } // i loop

    if ( (equivRel.size()+unRelated.size()) < 2)
        return std::list< NGroupPresentation* >();
// build return value. We'll have subgroup free products, and a free group
// provided unRelated is non-empty.

    std::list< NGroupPresentation* > retval;
    if (unRelated.size()>0) {
        NGroupPresentation* newGrp( new NGroupPresentation );
        newGrp->addGenerator( unRelated.size() );
        retval.push_back( newGrp );
    }
    for (std::list< std::set< unsigned long > >::iterator I=equivRel.begin();
            I!=equivRel.end(); I++) {
        NGroupPresentation* newGrp( new NGroupPresentation );
        newGrp->addGenerator( I->size() );
        std::map< unsigned long, unsigned long > downMap; // old to new map
        unsigned long count(0);
        for (std::set< unsigned long >::iterator J=I->begin(); J!=I->end(); J++) {
            downMap.insert( std::pair< unsigned long, unsigned long >(*J, count) );
            count++;
        }
        // Build map from this groups generators to corresponding generators of *this
        // decide which relators from *this are relevant.
        for (unsigned long i=0; i<relations.size(); i++) {
            // check if relations[i] generator is in *I
            if (I->find( relations[i]->getTerm(0).generator)!=I->end()) { // yes!
                NGroupExpression* newRel( new NGroupExpression );
                for (std::list<NGroupExpressionTerm>::const_iterator
                        ET=relations[i]->getTerms().begin();
                        ET!=relations[i]->getTerms().end(); ET++)
                    newRel->addTermLast( downMap[ ET->generator ], ET->exponent );
                newGrp->addRelation( newRel );
            }
        } // end relations i loop
        retval.push_back( newGrp );
    }
    return retval;
} // end identifyFreeProduct()

/**
 *  This algorithm is to recognise the fundamental groups of circle bundles
 * over surfaces, and determine (as best as it can) the type of circle bundle.
 *
 * Over S^2, there is S^1 x S^2, S^3, RP3 and lens spaces. These are the
 *  cyclic fundamental groups.
 *
 * Over RP^2 there is S^1 x RP^2 and S^2 x~ S^1 are the only two non-orientable
 *  manifolds, these are Z + Z_2 and Z.  The orientable manifolds are:
 * S^1 x~ RP^2, L_{4,1}, S^3/Q(8), SFS(S^2 | 1/2, -1/2, 1/3) and more generally
 *  SFS(S^2 | 1/2, -1/2, 1/k) == SFS( RP^2 | k ) with fundamental group
 *  < b, f | bfb^-1 = f^-1, b^2f^k > these are all finite groups. 
 *
 * Over S^1xS^1 there is...
 *
 * Over S^1 x~ S^1 there is...
 *
 * Over \Sigma_2 there is...
 *
 * Over \Sigma_{-3} there is...
 *
 * I'll extend the algorithm provided \Sigma_2 or \Sigma_{-3}'s are found
 *  in the census. 
 *
 * We should probably alter the return value to something more informative, 
 * eventually. 
 *
 * Will return an empty string if could not identify fundamental group in this
 * class. 
 */
std::string NGroupPresentation::identifyCircleBundleOverSurface(bool orientable)
{
 std::auto_ptr< NAbelianGroup > ab( abelianisation() );
 std::stringstream retval;

 // The simplest fundamental groups to recognise: S3, RP3, Lens.
 if (ab.get()->getRank()==0)
  {
   if (nGenerators==0) return std::string("S3");
   if (nGenerators==1) { // Lens space
     unsigned long ord( ab.get()->getInvariantFactor(0).longValue() );
     if (ord==2) return std::string("RP3");
     retval<<"L("<<ord<<",";
     if (ord<5) retval<<"1"; else retval<<"?";
     retval<<")";
     return retval.str();
    }
  }

 // next simplest: S2 x S1 and S2 x~ S1 (is bundle over RP2)
 if ( (ab.get()->getRank()==1) && (ab.get()->getNumberOfInvariantFactors()==0) )
  {
   if ( (nGenerators==1) && orientable ) return std::string("S2 x S1");
   if ( (nGenerators==1) && !orientable ) return std::string("S2 x~ S1");
  }

 // next, let's identify the bundles over RP2.  The non-orientable list
 // is just S2 x~ S1 non-orientable odd euler class (above)
 //     and S1 x RP2 non-orientable even euler class
 // the orientable list is more interesting, see next bracket
 if ( (ab.get()->getRank()==1) && (ab.get()->getNumberOfInvariantFactors()==1) )
  { // ab Z+Z_k
   if ( (ab.get()->getInvariantFactor(0).longValue()==2) && 
        isAbelian() ) return std::string("RP2 x S1");
  }

 // remaining non-or total space for S^1 bundles over RP2. 
 //       SFS(S^2 : 1/2, -1/2, 1/k) manifolds where k is the euler class, k>1 
 // equivalently SFS(RP^2 : k).  These have presentations
 //  < b, f : bfb^-1 = f^-1, b^2f^k=1 >.  These have abelianizations
 // Z_2 or Z_2^2 depending on whether or not k is odd or even, respectively
 if ( ( ab.get()->getRank()==0 ) && orientable ) {
  if ( ( ( ab.get()->getNumberOfInvariantFactors()==1 ) ?
       ( ab.get()->getInvariantFactor(0).longValue()==2 ) : false ) || 
       ( ( ab.get()->getNumberOfInvariantFactors()==2 ) ?
       ( ( ab.get()->getInvariantFactor(0).longValue()==2 ) &&
         ( ab.get()->getInvariantFactor(1).longValue()==2 ) ) : false ) )
   {
    for (unsigned long k=2; k<6; k++)
     { // TODO: this is very primitive and limited to euler char < 7. 
      std::stringstream tempstr; 
      std::vector<std::string> relVec; relVec.push_back("abAb");
      tempstr<<"aab^"<<k; relVec.push_back(tempstr.str());
      NGroupPresentation target(2, relVec);
      target.intelligentSimplify();
      target.proliferateRelators();
      tempstr.flush();
      if (k==2) tempstr<<"S3 / Q8";
      else      tempstr<<"S1 x~"<<k<<" RP2";
      if (isSimpleIsomorphicTo(target)) return tempstr.str();
     }
   }
 }

 // bundles over S1 x S1, and S1 x~ S1
 if ( isAbelian() && (ab.get()->getRank()==3) && 
                     (ab.get()->getNumberOfInvariantFactors()==0) )
   return std::string("S1 x S1 x S1");

 return std::string();
}

// Routine (at present) looks for isomorphism between *this group and other,
// at present only maps of the form that send generators
//  g_i --> g_{sigma i}^{delta_i} where sigma is some permutation of the
// generators, and delta is some function {0,1,...,ngens-1} --> {+1, -1}
//
// We do this by creating a routine that runs through the relators of this
// group and checks if there are any partial permutations sigma that allow
// that relator to be respected by a map.  It builds up a big list of all 
// these partial subs, one list for every relator in *this group.  As we
// iterate through the relators we iteratively check compatibility of these
// subsititions lists, winnowing-down the list of substitions as we go.
// Once done, if non-empty that would define the map
// on all generators other than free factors, so then we have to similarly
// check for free factors in other.  Then we check the inverse (in the free
// group) descends to a map, if so we're done. 
//
// To enable this we should probably carefully index the relations. And we
// should handle 1-gen relations differently than multiple-gen relations, 
// otherwise there's a potential memory explosion.  
//
// TODO: we can modify this to be a findHom routine. And if the target is 
// a finite group, find *all* homs up to conjugacy, etc. 
//
bool NGroupPresentation::isSimpleIsomorphicTo( const NGroupPresentation& other )
const
{
// Check if presentations have the same number of generators.
    if (nGenerators != other.nGenerators) return false;
// Check if relations empty
    if ( (relations.size()==0) && (other.relations.size()==0) ) return true;
    if ( (relations.size()==0) || (other.relations.size()==0) ) return false;
// Both relations.size()>0, and have the same number of generators.

// list of related by number of generators appearing
    std::map< unsigned long, std::list< NGroupExpression* > > domRelIdx, ranRelIdx;

    for (unsigned long i=0; i<relations.size(); i++) {
        const std::list<NGroupExpressionTerm> nget( relations[i]->getTerms() );
        std::set< unsigned long > gensUsed;
        for (std::list<NGroupExpressionTerm>::const_iterator j=nget.begin();
                j!=nget.end(); j++)
            gensUsed.insert( j->generator );
        domRelIdx[ gensUsed.size() ].push_back( relations[i] );
    }
    for (unsigned long i=0; i<other.relations.size(); i++) {
        const std::list<NGroupExpressionTerm> nget( other.relations[i]->getTerms() );
        std::set< unsigned long > gensUsed;
        for (std::list<NGroupExpressionTerm>::const_iterator j=nget.begin();
                j!=nget.end(); j++)
            gensUsed.insert( j->generator );
        ranRelIdx[ gensUsed.size() ].push_back( other.relations[i] );
    }

// for each relator of this we have lists of potential substitutions
// typedef std::list< partialSubType > pSubListType;
    std::list< std::map<unsigned long, NGroupExpressionTerm> > allPartialSubs;
    allPartialSubs.push_back( std::map<unsigned long, NGroupExpressionTerm>() );

    for (std::map< unsigned long, std::list< NGroupExpression* > >::const_reverse_iterator
            i=domRelIdx.rbegin(); i!=domRelIdx.rend(); i++) {
        // currently we'll do the most simplistic thing possible -- look for relabellings
        // of these relators in the target presentation.
        unsigned long nGens = i->first;
        if (ranRelIdx.find(nGens)==ranRelIdx.end()) return false;

        const std::list< NGroupExpression* > DOMR( i->second );
        const std::list< NGroupExpression* > RANR( ranRelIdx[nGens] );
        // build list of subs for all DOMR -> RANR possibilities
        for (std::list< NGroupExpression* >::const_iterator DI=DOMR.begin();
                DI!=DOMR.end(); DI++) {
            std::list< std::map<unsigned long, NGroupExpressionTerm> > newPartialSubs;
            // for each DI, every extension or consistent hom with allPArtialSubs
            // we find using DI will be put in newPartialSubs, at the end, we replace
            // allPartialSubs with newPartialSubs.
            for (std::list< NGroupExpression* >::const_iterator RI=RANR.begin();
                    RI!=RANR.end(); RI++) {
                // built tempList
                // TODO: let's put the special case nGens==1 here, where instead of
                //  looking making all possible maps, we just choose one.  This is
                //  because if we get here and its not defined on a torsion element,
                //  is must have been a free factor Z_k * other stuff.  So we only
                //  need to choose a complementary map.

                std::list< std::map< unsigned long, NGroupExpressionTerm > >
                tempList( (*DI)->relabellingsThisToOther( *(*RI), true ) );
                for (std::list< std::map< unsigned long, NGroupExpressionTerm > >::iterator
                        X=tempList.begin(); X!=tempList.end(); X++)
                    for (std::list< std::map< unsigned long, NGroupExpressionTerm > >::iterator
                            Y=allPartialSubs.begin(); Y!=allPartialSubs.end(); Y++) {
                        // newMap will be the potential extension of *X and *Y
                        std::map< unsigned long, NGroupExpressionTerm > newMap;
                        std::map< unsigned long, NGroupExpressionTerm >::iterator Xi=X->begin();
                        std::map< unsigned long, NGroupExpressionTerm >::iterator Yi=Y->begin();
                        while ( (Xi!=X->end()) || (Yi!=Y->end()) ) {
                            if ( (Xi!=X->end()) && (Yi!=Y->end()) ) {
                                if (Xi->first < Yi->first) {
                                    newMap.insert( *Xi );
                                    Xi++;
                                } else if (Xi->first > Yi->first) {
                                    newMap.insert( *Yi );
                                    Yi++;
                                } else if (Xi->second == Yi->second) {
                                    newMap.insert( *Xi );
                                    Xi++;
                                    Yi++;
                                } else // this does not extend
                                    goto get_out_of_while_loop_goto_tag;
                            } else if (Xi!=X->end()) {
                                newMap.insert( *Xi );
                                Xi++;
                            } else if (Yi!=Y->end()) {
                                newMap.insert( *Yi );
                                Yi++;
                            }
                        }
                        newPartialSubs.push_back( newMap );
get_out_of_while_loop_goto_tag: { // this skips insertion
                        } // goto tags apparently can't be at the end of while loops!
                    }
            } // end RI loop
            if (newPartialSubs.empty()) return false;
            allPartialSubs.clear();
            allPartialSubs.swap( newPartialSubs );
            // TODO: Remove duplicates if they exist. This would help reduce time
            //  wasted
        } // end DI and newPartialSubs loop
    }

// TODO: if still undefined, there are some free factors.  Count them on
//  both sides then define.

    for (std::list< std::map< unsigned long, NGroupExpressionTerm > >::iterator
            X=allPartialSubs.begin(); X!=allPartialSubs.end(); X++) {
        unsigned long gi=0;
        std::set< unsigned long > rGen;
        for (std::map<unsigned long, NGroupExpressionTerm >::iterator GI=X->begin();
                GI!=X->end(); GI++) {
            rGen.insert(GI->second.generator);
            if (GI->first!=gi) break;
            else gi++;
        }
        if ( (rGen.size()==nGenerators) && (gi==nGenerators) ) {
            std::vector< NGroupExpression > map(nGenerators);
            for (std::map<unsigned long, NGroupExpressionTerm >::iterator GI=X->begin();
                    GI!=X->end(); GI++) {
                NGroupExpression let;
                let.addTermFirst( GI->first, GI->second.exponent );
                map[GI->second.generator] = let;
            }
            NHomGroupPresentation invMap( other, *this, map );
            if (invMap.verifyHom()) { //std::cout<<invMap.detail()<<"\n";
                return true;
            }
        }
    }

    return false;
}


bool NGroupPresentation::nielsenTransposition(const unsigned long &i,
        const unsigned long &j)
{
    if (i==j) return false;
    bool retval=false;
    for (unsigned long l=0; l<relations.size(); l++) {
        std::list<NGroupExpressionTerm>& terms( relations[l]->getTerms() );
        for (std::list<NGroupExpressionTerm>::iterator k=terms.begin();
                k!=terms.end(); k++) {
            if (k->generator == i) {
                k->generator = j;
                retval = true;
            } else if (k->generator == j) {
                k->generator = i;
                retval = true;
            }
        }
    }
    return retval;
}

bool NGroupPresentation::nielsenInvert(const unsigned long &i)
{
    bool retval=false;
    for (unsigned long l=0; l<relations.size(); l++) {
        std::list<NGroupExpressionTerm>& terms(relations[l]->getTerms());
        for (std::list<NGroupExpressionTerm>::iterator k=terms.begin();
                k!=terms.end(); k++) {
            if (k->generator == i) {
                k->exponent=(-k->exponent);
                retval = true;
            }
        }
    }
    return retval;
}

bool NGroupPresentation::nielsenCombine(const unsigned long &i,
                                        const unsigned long &j,
                                        const signed long &k,
                                        const bool &flag)
{
    // replace ri with (ri)(rj)^(-k)
    bool retval(false);
    NGroupExpression let;
    if (flag) {
        let.addTermFirst(i, 1);
        let.addTermLast(j, -k);
    } else {
        let.addTermLast(i, 1);
        let.addTermFirst(j, -k);
    }
    for (unsigned long k=0; k<relations.size(); k++)
        if (relations[k]->substitute(i, let, true)) retval = true;
    return retval;
}

// these macros are used only in the identifyExtensionOverZ routine below.
namespace { // anonymous namespace to ensure not put in the library
#define idx(gen, cov) ((unsigned long)(gen-1)+nGm1*cov)
#define unidx(dat) std::pair<unsigned long, unsigned long>((dat % nGm1)+1, dat/nGm1)
}

// if presentation is of a group that can bet written as an extension
//  0 --> A --> G --> Z --> 0
// this routine is to change the presentation to appear to be such a split
//  extension. 
//
// TODO: at present it will not declare presentations of the form:
//  < a, b | a^5, bab^-1=a^2 > extensions over Z, because of the 
// a^2 term.  Should fix this.  But how to do it in any generality? 
// Perhaps multiply conjugating automorphisms, to deduce
//  < a, b | a^5, bab^-1=a^2, ba^2b^-1=a^4=a^-1 > etc...
// Short-term we can recognise the fibre as being abelian and check
// invertibility using HomMarkedAbelianGroup routines.
//
std::auto_ptr< NHomGroupPresentation >
NGroupPresentation::identifyExtensionOverZ()
{
// step 1: let's build the abelianization homomorphism.
    homologicalAlignment();
    std::auto_ptr< NMarkedAbelianGroup > abelianized( markedAbelianisation() );
    if (abelianized->getRank() != 1) return
            std::auto_ptr< NHomGroupPresentation >(NULL);
    if (abelianized->getNumberOfInvariantFactors()>0)  // put Z generator at 0-th
        nielsenTransposition(0, abelianized->getNumberOfInvariantFactors() );

// We have the presentation of this group in the form
// < a, g1, g2, ..., gn | r1, ..., rm > with a->1, gi->0 under abelianization
//
// step 2: An infinite presentation of the kernel of the map to Z is given
//  by < g1i, g2i, ..., gni | r1i, ..., rmi > for all lifts i of the generators
//  and relators above, after collapsing "a". We can collapse this to a finite
//  presentation if and only if unique max and minima (in the Z cover) exist
//  among the lifted relators.  So we check for that.

// lifts stores the lifts of the ri's, after crushing the lifts of the a's.
    std::vector< std::list< std::pair< NGroupExpressionTerm, signed long > > >
    lifts( relations.size() );

// the following max/minKiller give a list of the found pairs
//  (generator index, relator index) to keep track of which relators we can
//  use to kill generators (in the covering space).
    std::map< unsigned long, unsigned long > maxKiller;
    std::map< unsigned long, unsigned long > minKiller;
    std::map< unsigned long, unsigned long > cellWidth; // 2-cell width in cover

    for (unsigned long l=0; l<relations.size(); l++) {
        // for each relator determine highest and lowest lifts, and if they
        // are unique or not.

        signed long lift=0;
        signed long maxLift(0), minLift(0);   // sheet index
        unsigned long maxCell(0), minCell(0); // generator's index in presentation
        bool dupMax(false), dupMin(false);    // duplicate lift height?
        std::list<NGroupExpressionTerm>& terms(relations[l]->getTerms());
        for (std::list<NGroupExpressionTerm>::reverse_iterator k=terms.rbegin();
                k!=terms.rend(); k++) {
            // right to left through the relator
            if (k->generator > 0) {
                lifts[l].push_back( std::pair< NGroupExpressionTerm, signed long >
                                    ( *k, lift ) );
                // special case if maxCell and minCell not yet initialized.
                if (maxCell==0) {
                    maxLift = lift;
                    minLift = lift;
                    maxCell = k->generator;
                    minCell = k->generator;
                    dupMax = (labs(k->exponent)==1) ? false : true;
                    dupMin = (labs(k->exponent)==1) ? false : true;
                } else { // back to regular case
                    if (lift > maxLift) {
                        maxLift = lift;
                        dupMax = (labs(k->exponent)==1) ? false : true;
                        maxCell = k->generator;
                    } else if (lift==maxLift) {
                        dupMax = true;
                    }
                    if (lift < minLift) {
                        minLift = lift;
                        dupMin = (labs(k->exponent)==1) ? false : true;
                        minCell = k->generator;
                    } else if (lift==minLift) {
                        dupMin = true;
                    }
                }
            } else lift += k->exponent;
        }
        // maxCell and minCell have to be non-zero at this point.
        cellWidth[l] = (unsigned long)(maxLift - minLift);

        if ( (maxCell!=0) && (!dupMax) ) {
            std::map< unsigned long, unsigned long>::iterator
            I=maxKiller.find(maxCell);
            if (I!=maxKiller.end()) { // compare the current maxKiller[maxCell] to l.
                if (cellWidth[l] > cellWidth[ I->second ]) maxKiller[maxCell]=l;
            } else maxKiller[maxCell]=l;
        }
        if ( (minCell!=0) && (!dupMin) ) {
            std::map< unsigned long, unsigned long>::iterator
            I=minKiller.find(minCell);
            if (I!=minKiller.end()) { // compare the current maxKiller[minCell] to l.
                if (cellWidth[l] > cellWidth[ I->second ]) minKiller[minCell]=l;
            }
            minKiller[minCell]=l;
        }
        // now let's readjust the relator so that its minLift is at level 0.
        if (minLift != 0) {
            relations[l]->addTermFirst(0, minLift);
            relations[l]->addTermLast(0, -minLift);
            relations[l]->simplify();
            for (std::list<std::pair< NGroupExpressionTerm, signed long> >::iterator
                    I=lifts[l].begin(); I!=lifts[l].end(); I++)
                I->second -= minLift; // adjust the lifts to have min lift 0
        }
        // cyclically permute lifts so that the max-weight rep appears first
        while (lifts[l].front().second != cellWidth[l]) {
            std::pair< NGroupExpressionTerm, signed long > temp(lifts[l].front());
            lifts[l].pop_front();
            lifts[l].push_back( temp );
        }
        // ensure word starts with highest-weight element as inverted.
        if (lifts[l].front().first.exponent == 1) {
            std::pair< NGroupExpressionTerm, signed long > temp(lifts[l].front());
            lifts[l].pop_front();
            lifts[l].reverse();
            lifts[l].push_front(temp); // now run and change the exponents
            for (std::list< std::pair<NGroupExpressionTerm, signed long> >::iterator
                    I=lifts[l].begin(); I!=lifts[l].end(); I++)
                I->first.exponent = -I->first.exponent;
        }
    }
// this is the test for whether or not we can find a finite collection of
// generators
    unsigned long nGm1( nGenerators - 1 );
    if ( (maxKiller.size() != nGm1) || (minKiller.size() != nGm1) )
        return std::auto_ptr< NHomGroupPresentation >(NULL);

    unsigned long maxWidth(0);
    unsigned long liftCount(0); // how many lifts of our generators do we need?
    for (unsigned long i=0; i<maxKiller.size(); i++) {
        if (cellWidth[maxKiller[i]]>liftCount) liftCount = cellWidth[maxKiller[i]];
        if (cellWidth[minKiller[i]]>liftCount) liftCount = cellWidth[minKiller[i]];
    }
    for (std::map< unsigned long, unsigned long>::iterator I=cellWidth.begin();
            I!=cellWidth.end(); I++)
        if (I->second > maxWidth) maxWidth = I->second;
// we need liftCount lifts of our generators and relators.  Perhaps we should
// either cite something in Magnus-Karass-Solitar for this or put in a proof.
// let's build a vector that describes the relationa a(gi)a^-1 = ...

// build table of reductions of the liftCount==M lift of generators.
// the indexing of the generators of the kernel of G --> Z will be handled
// by the above idx and unidx macros.
    std::map<unsigned long, NGroupExpression> genKiller;
// start with the liftCount lift, i.e. the first order reducers a^-Mg_ia^M =...
    for (unsigned long i=1; i<getNumberOfGenerators(); i++) {
        NGroupExpression temp;
        // maxKiller[i] is the index in lifts of the relator that kills generator gi
        // i is a liftIdx
        unsigned long delta(0);
        for (std::list< std::pair< NGroupExpressionTerm, signed long > >::iterator
                I=lifts[maxKiller[i]].begin(); I!=lifts[maxKiller[i]].end(); I++) {
            if (I==lifts[maxKiller[i]].begin()) {
                // push up delta sheets so that it kills appropriately
                delta = (unsigned long)(liftCount - I->second);
                continue;
            }
            temp.addTermFirst( NGroupExpressionTerm(
                                   idx( I->first.generator, I->second + delta ), I->first.exponent ) );
        }
        genKiller.insert( std::pair< unsigned long, NGroupExpression >
                          (idx(i,liftCount),temp) );
    }

// extra genKillers -- sometimes there are wider words than the killing words.
//  like with presentations such as:
//   < a b | b a^-1 b a^-1 b^-1 a^2, a^-3 b^2 a^3 b^2 >
// We could alternatively use the genKiller to reduce the width of the other
// relators.  But for now we use this less-sophisticated work-around.
    for (unsigned long j=liftCount; j<maxWidth; j++) {
        for (unsigned long i=1; i<getNumberOfGenerators(); i++) {
            // bump-up lift of each genKiller then apply previous genKillers to them
            // to create word in the fibre group.
            NGroupExpression tempW( genKiller[idx(i, j)] );
            for (std::list<NGroupExpressionTerm>::iterator I=tempW.getTerms().begin();
                    I!=tempW.getTerms().end(); I++) I->generator += nGm1;
            for (std::map<unsigned long, NGroupExpression>::iterator
                    J=genKiller.begin(); J!=genKiller.end(); J++)
                tempW.substitute( J->first, J->second, false );
            genKiller.insert( std::pair< unsigned long, NGroupExpression >
                              (idx(i,j+1), tempW) );
        }
    }

//  initialize tempTable with the 0-th lifts of the relators.
    std::list< NGroupExpression > tempTable;
    NGroupPresentation kerPres;
    kerPres.addGenerator( liftCount * nGm1 );

    for (unsigned long i=0; i<lifts.size(); i++) {
        NGroupExpression temp;
        for (std::list< std::pair< NGroupExpressionTerm, signed long >
                >::iterator I=lifts[i].begin(); I!=lifts[i].end(); I++)
            temp.addTermFirst( NGroupExpressionTerm(
                                   idx( I->first.generator, I->second ), I->first.exponent ) );
        for (std::map<unsigned long, NGroupExpression>::iterator J=genKiller.begin();
                J!=genKiller.end(); J++)
            temp.substitute( J->first, J->second, false );
        temp.simplify();
        if (temp.wordLength()>0) {
            tempTable.push_back(temp);
            kerPres.addRelation( new NGroupExpression(temp) );
        }
    }
    if (!kerPres.isValid()) {
        std::cout<<"identifyExtensionOverZ() error:"<<
                 " out of bounds relator in kerPres.\n";
        std::cout.flush();
        exit(1);
    }
// build the reductions of the {0,1,...,liftCount-1} translates of **all**
// the relators from the group, and assemble them into the relators of the
// kernel.
    for (unsigned long M=0; M<liftCount; M++) {
        // increment the words in tempTable
        for ( std::list< NGroupExpression >::iterator I=tempTable.begin();
                I != tempTable.end(); I++) {
            std::list< NGroupExpressionTerm >& It(I->getTerms() );
            for (std::list<NGroupExpressionTerm>::iterator J=It.begin();
                    J!=It.end(); J++)
                J->generator += nGm1; // this depends on choice of idx function
            for (std::map<unsigned long, NGroupExpression>::iterator
                    J=genKiller.begin(); J!=genKiller.end(); J++)
                I->substitute( J->first, J->second, false );
            // apply genKiller to reduce the words, and push to presentation
            kerPres.addRelation( new NGroupExpression( *I ) );
        }
    }
    // replace this presentation by the semi-direct product presentation.
    std::vector<NGroupExpression> autVec;

    autVec.resize( nGm1*liftCount );
    for (unsigned long i=0; i<autVec.size(); i++) // this part depends on idx
        if ( i >= nGm1*(liftCount-1) )
            autVec[i] = genKiller[i+nGm1];
        else {
            NGroupExpression temp;
            temp.addTermFirst( i+nGm1, 1 );
            autVec[i] = temp;
        }

    std::auto_ptr< NHomGroupPresentation >
    retval(new NHomGroupPresentation( kerPres, kerPres, autVec ) );
    retval->intelligentSimplify();

    // Modify this presentation to reflect the semi-direct product
    //        structure we've discovered!
    // deallocate relations, and resize and repopulate with copies of kerPres's
    //  relations.

    nGenerators = retval->getDomain().nGenerators + 1;
    for (unsigned long i=0; i<relations.size(); i++)
        delete relations[i];
    relations.resize( retval->getDomain().nGenerators +
                      retval->getDomain().relations.size() );

    for (unsigned long i=0; i<retval->getDomain().relations.size(); i++)
        relations[i] = new NGroupExpression( *retval->getDomain().relations[i] );

    // And now all the b^-1g_ib = genKiller(i) and b^-1g_ib = g_{i+1} relations.
    for (unsigned long i=0; i<retval->getDomain().nGenerators; i++) {
        NGroupExpression* temp;
        temp = new NGroupExpression( retval->evaluate(i) );
        temp->addTermFirst( retval->getDomain().nGenerators, 1);
        temp->addTermFirst( i, -1);
        temp->addTermFirst( retval->getDomain().nGenerators, -1);
        relations[ i+retval->getDomain().relations.size() ] = temp;
    }

    return std::auto_ptr< NHomGroupPresentation >( retval );
}


bool NGroupPresentation::isValid() const
{
    for (unsigned long i=0; i<relations.size(); i++)
     for (std::list<NGroupExpressionTerm>::const_iterator 
            j=relations[i]->getTerms().begin(); 
            j!=relations[i]->getTerms().end(); j++)
      if (j->generator >= nGenerators) return false;
    return true;
}


/*
 * This is an entirely cosmetic re-writing of the presentation, is
 * fast and superficial.   
 *  1) It inverts relators if net sign of the generators is negative.
 *  2) It cyclically permutes relators to start with smallest gen.
 *  3) It sorts the relators by number of generator indices that
 *     appear, followed by relator numbers (lexico) followed by 
 *     relator length. 
 *  4) Makes elementary simplifications to aid in seeing standard
 *     relators like commutators. 
 */
namespace { // anonymous namespace
    bool compare_words(const NGroupExpression* first,
                       const NGroupExpression* second)
    {
    // compute number of letters used
        std::set<unsigned long> usedTermsF, usedTermsS;
        for (std::list<NGroupExpressionTerm>::const_iterator
                j=first->getTerms().begin();  j!=first->getTerms().end(); j++)
            usedTermsF.insert( j->generator );
        for (std::list<NGroupExpressionTerm>::const_iterator
                j=second->getTerms().begin(); j!=second->getTerms().end(); j++)
            usedTermsS.insert( j->generator );
        if (usedTermsF.size() < usedTermsS.size()) return true;
        if (usedTermsF.size() > usedTermsS.size()) return false;
    // have all the same terms, sort lexicographically on which terms used
        std::set<unsigned long>::iterator i=usedTermsF.begin();
        std::set<unsigned long>::iterator j=usedTermsS.begin();
        while (i != usedTermsF.end()) {
            if (*i < *j) return true;
            if (*i > *j) return false;
            i++;
            j++;
        }
    // the two words first and second use exactly the same letters.
        if (first->wordLength() < second->wordLength()) return true;
        if (first->wordLength() > second->wordLength()) return false;
        if (first->getNumberOfTerms() < second->getNumberOfTerms()) return true;
        if (first->getNumberOfTerms() > second->getNumberOfTerms()) return false;
    // now we can compare them lexicographically, letter by letter.
    // first we lay them out one letter at a time.
        std::vector< NGroupExpressionTerm > first_word_vec( 0 );
        std::vector< NGroupExpressionTerm > second_word_vec( 0 );
        first_word_vec.reserve( first->wordLength() );
        second_word_vec.reserve( second->wordLength() );
        std::list<NGroupExpressionTerm>::const_iterator it;
        for (it = first->getTerms().begin(); it!=first->getTerms().end(); it++) {
            for (unsigned long I=0; I<abs((*it).exponent); I++)
                first_word_vec.push_back( NGroupExpressionTerm( (*it).generator,
                                          ((*it).exponent>0) ? 1 : -1 ) );
        }
        for (it = second->getTerms().begin(); it!=second->getTerms().end(); it++) {
            for (unsigned long I=0; I<abs((*it).exponent); I++)
                second_word_vec.push_back( NGroupExpressionTerm( (*it).generator,
                                           ((*it).exponent>0) ? 1 : -1 ) );
        }
    // now we compare termwise
        for (unsigned long I=0; I<first_word_vec.size(); I++) {
            if (first_word_vec[I].generator < second_word_vec[I].generator) return true;
            if (first_word_vec[I].generator > second_word_vec[I].generator) return false;
            if (first_word_vec[I].exponent  < second_word_vec[I].exponent) return true;
            if (first_word_vec[I].exponent  > second_word_vec[I].exponent) return false;
        }

    // exactly the same words.
        return false;
    }
} // end anonymous namespace

bool NGroupPresentation::prettyRewriting()
{ return prettyRewritingDetail().get(); }

// this routine iteratively finds length 1 relators, and uses them to simplify
// other relators.  In the end it deletes all length 0 relators and re-indexes. 
std::auto_ptr<NHomGroupPresentation> NGroupPresentation::prettyRewritingDetail()
{
// keep the relators in a list for now.
    std::list<NGroupExpression*> relatorPile;
    for (unsigned long i=0; i<relations.size(); i++)
        relatorPile.push_back( relations[i] );
    NGroupPresentation oldPres(*this);

// step 1: cyclic reduce relators. Delete length 0 relators.
//         delete generators corresponding to length 1 relators
    std::list<NGroupExpression*>::iterator it;
    for ( it = relatorPile.begin(); it != relatorPile.end(); it++ )
        (*it)->simplify(true);

    std::set<unsigned long> genToDel; // generators we've eliminated
    bool reloopFlag(true);
    while (reloopFlag) {
        reloopFlag=false;
        std::set<unsigned long> newGenDel;
        for ( it = relatorPile.begin(); it != relatorPile.end(); it++ ) {
            if ((*it)->getNumberOfTerms() == 1)
                if ( abs( (*it)->getTerms().front().exponent ) == 1 ) { // a killer!
                    newGenDel.insert( (*it)->getTerms().front().generator );
                }
        }
        genToDel.insert( newGenDel.begin(), newGenDel.end() );

        for (std::set<unsigned long>::iterator i=newGenDel.begin();
                i!=newGenDel.end(); i++)
            for (it=relatorPile.begin(); it!=relatorPile.end(); it++)
                if ((*it)->substitute(*i, NGroupExpression(), true ))
                    reloopFlag=true;
    }

    relations.clear();
    relations.reserve( relatorPile.size() - genToDel.size() );
    for (it = relatorPile.begin(); it!=relatorPile.end(); it++) {
        if ( (*it)->getNumberOfTerms()>0 )
            relations.push_back( *it );
        else delete (*it);
    }
    relatorPile.clear();
    for (unsigned long i=0; i<relations.size(); i++)
        relatorPile.push_back( relations[i] );

    std::auto_ptr< NHomGroupPresentation > redMap;
    if (genToDel.size()>0) {
        std::set< unsigned long > interval, compDelete; // complement
        for (unsigned long i=0; i<nGenerators; i++)
            interval.insert(interval.end(), i);
        std::set_difference( interval.begin(), interval.end(),
                             genToDel.begin(), genToDel.end(),
                             std::inserter(compDelete, compDelete.end() ) );

        // then reduce the group, run through genToDel and do gi->1 subs on all
        //  relators, and gk --> gk-1 for larger generators.
        std::vector< NGroupExpression > downSub(nGenerators);
        std::vector< NGroupExpression > upSub(nGenerators - genToDel.size());
        unsigned long i=0;
        for (std::set<unsigned long>::iterator I=compDelete.begin();
                I!=compDelete.end(); I++) {
            upSub[i].addTermFirst( NGroupExpressionTerm( (*I), 1 ) );
            downSub[*I].addTermFirst( NGroupExpressionTerm( i, 1 ) );
            // might as well perform downSub now on all relators.
            for (it=relatorPile.begin(); it!=relatorPile.end(); it++)
                (*it)->substitute( *I, downSub[*I], true);
            i++;
        }
        nGenerators -= genToDel.size();
        // assemble the reduction map
        redMap.reset( new NHomGroupPresentation( oldPres, *this,
                      downSub, upSub ) );
    }

// step 2: sort by number of letters present, followed by word length
//         to do this, we build a list of relators and a sorting criterion.
    relatorPile.sort(compare_words);
    relations.clear();
    relations.reserve(relatorPile.size());
    for (it = relatorPile.begin(); it!=relatorPile.end(); it++)
        relations.push_back( *it );

// step 3: if there is a relator with net sign < 0, invert it
    for (unsigned long i=0; i<relations.size(); i++) {
        // add up signs
        signed int sig(0);
        for (std::list<NGroupExpressionTerm>::iterator
                j=relations[i]->getTerms().begin();
                j!=relations[i]->getTerms().end(); j++) sig += j->exponent;
        if (sig < 0) relations[i]->invert();
    }

// step 4: for each generator find first relator where it appears with non-zero
//   weight, if negative, invert the generator.
// TODO maybe...

// step 5: cyclically permute to start with a lexicographically smallest
//         term
    for (unsigned long i=0; i<relations.size(); i++)
        if (relations[i]->getNumberOfTerms()>0) {
            // form list of all terms involved, find smallest, cyclically permute to
            //  start with that one.
            std::set<unsigned long> usedTerms;
            for (std::list<NGroupExpressionTerm>::iterator
                    j=relations[i]->getTerms().begin();
                    j!=relations[i]->getTerms().end(); j++)
                usedTerms.insert( j->generator );
            unsigned long smallestGen( *usedTerms.begin() );
            while ( relations[i]->getTerm(0).generator != smallestGen )
                relations[i]->cycleRight();
        }

    return std::auto_ptr<NHomGroupPresentation>( redMap.release() );
}


////////////////// ALL INPUT / OUTPUT routines below //////////////////////

// XML output

void NGroupPresentation::writeXMLData(std::ostream& out) const {
    out << "<group generators=\"" << nGenerators << "\">\n";
    for (RelIteratorConst it = relations.begin(); it != relations.end(); it++) {
        out << "  ";
        (*it)->writeXMLData(out);
        out << '\n';
    }
    out << "</group>\n";
}

void NGroupExpression::writeXMLData(std::ostream& out) const {
    out << "<reln> ";
    for (TermIteratorConst it = terms.begin(); it != terms.end(); it++)
        out << (*it).generator << '^' << (*it).exponent << ' ';
    out << "</reln>";
}

// group expression output routines

void NGroupExpression::writeText(std::ostream& out, bool shortword) const {
    if (terms.empty())
        out << '1';
    else {
        std::list<NGroupExpressionTerm>::const_iterator i;
        for (i = terms.begin(); i!=terms.end(); i++) {
            if (i != terms.begin())
                out << ' ';
            if (shortword)
                out << char('a' + i->generator);
            else
                out << "g_" << i->generator;
            if ( i->exponent != 1 )
                out << '^' << i->exponent;
        }
    }
}

std::string NGroupExpression::toTeX() const {
    std::ostringstream out;
    writeTeX(out);
    return out.str();
}

void NGroupExpression::writeTeX(std::ostream& out) const {
    if (terms.empty())
        out << 'e';
    else {
        std::list<NGroupExpressionTerm>::const_iterator i;
        for (i = terms.begin(); i!=terms.end(); i++) {
            out << "g_{" << i->generator << '}';
            if ( i->exponent != 1 )
                out << "^{" << i->exponent << '}';
        }
    }
}

void NGroupExpression::writeTextShort(std::ostream& out) const {
    if (terms.empty())
        out << '1';
    else {
        TermIteratorConst last = --terms.end();
        copy(terms.begin(), last,
            std::ostream_iterator<NGroupExpressionTerm>(out, " "));
        out << *last;
    }
}

// presentation output routines below

std::string NGroupPresentation::toTeX() const {
    std::ostringstream out;
    writeTeX(out);
    return out.str();
}

void NGroupPresentation::writeTeX(std::ostream& out) const {
    out << "\\langle ";
    if (nGenerators == 0) out << "\\cdot";
    else if (nGenerators == 1) out << "g_0";
    else if (nGenerators == 2) out << "g_0, g_1";
    else {
        out << "g0, \\cdots, g" <<(nGenerators - 1);
    }
    out << " | ";
    if (relations.empty())
        out << "\\cdot";
    else
        for (RelIteratorConst it = relations.begin();
                it != relations.end(); it++) {
            if (it != relations.begin())
                out << ", ";
            (*it)->writeTeX(out);
        }
    out << " \\rangle";
}

void NGroupPresentation::writeTextLong(std::ostream& out) const {
    out << "Generators: ";
    if (nGenerators == 0)
        out << "(none)";
    else if (nGenerators == 1)
        out << "a";
    else if (nGenerators == 2)
        out << "a, b";
    else if (nGenerators <= 26)
        out << "a .. " << char('a' + nGenerators - 1);
    else
        out << "g0 .. g" << (nGenerators - 1);
    out << std::endl;

    out << "Relations:\n";
    if (relations.empty())
        out << "    (none)\n";
    else
        for (RelIteratorConst it = relations.begin();
                it != relations.end(); it++) {
            out << "    ";
            (*it)->writeText(out, nGenerators <= 26);
            out << std::endl;
        }
}

std::string NGroupPresentation::toStringCompact() const {
    std::ostringstream out;
    writeTextCompact(out);
    return out.str();
}

std::string NGroupPresentation::compact() const {
    std::ostringstream out;
    writeTextCompact(out);
    return out.str();
}

void NGroupPresentation::writeTextCompact(std::ostream& out) const {
    if (nGenerators == 0) {
        out << "< >";
        return;
    }

    out << "<";
    if (nGenerators <= 26) {
        for (unsigned long i=0; i<nGenerators; i++)
            out << ' ' << char('a' + i);
    } else {
        out << " g0 .. g" << (nGenerators - 1);
    }

    if (relations.empty()) {
        out << " >";
        return;
    }

    out << " | ";
    for (RelIteratorConst it = relations.begin();
            it != relations.end(); it++) {
        if (it != relations.begin())
            out << ", ";
        (*it)->writeText(out, nGenerators <= 26);
    }
    out << " >";
}


// We will go through, apply dehnAlgSubMetric look for substitutions, 
//  then apply all of them within a reasonable length. 
// if user requests we will go further and do a 2nd iteration with more care...
// depth==1 by default. 

void NGroupPresentation::proliferateRelators(unsigned long depth)
{
    std::list< NGroupExpression* > newRels;
    for (unsigned long i=0; i<relations.size(); i++)
        for (unsigned long j=0; j<relations.size(); j++) {
            if (i==j) continue; // TODO: maybe accept novel self-substitutions?
            std::set< NWordSubstitutionData > sub_list;
            dehnAlgorithmSubMetric( *(relations[i]), *(relations[j]), sub_list, depth );
            while (!sub_list.empty()) {
                NGroupExpression* newRel( new NGroupExpression( *(relations[i]) ) );
                applySubstitution( *newRel, *(relations[j]), *sub_list.begin() );
                sub_list.erase( sub_list.begin() );
                newRels.push_back(newRel);
            }
        }
    depth--;
    while (depth>0) {
        std::list< NGroupExpression* > tempRels;
        for (unsigned long i=0; i<relations.size(); i++)
            for (std::list< NGroupExpression* >::iterator j=newRels.begin();
                    j!=newRels.end(); j++) {
                // attempt to tack relation[i] to *j. To do this, we should perhaps
                // keep a record of how *j was created, as in where the two junction
                // points are so as to ensure what we're adding spans at least one
                // of the junctions.
                std::set< NWordSubstitutionData > sub_list;
                dehnAlgorithmSubMetric( **j, *(relations[i]), sub_list, depth );
                while (!sub_list.empty()) {
                    // TODO: we might want to avoid some obviously repetitive subs
                    //       as noted above?
                    NGroupExpression* newRel( new NGroupExpression( *(*j) ) );
                    applySubstitution( *newRel, *(relations[i]), *sub_list.begin() );
                    sub_list.erase( sub_list.begin() );
                    tempRels.push_back(newRel);
                }
            }
        depth--;
        while (!tempRels.empty()) {
            newRels.push_back( *tempRels.begin() );
            tempRels.erase( tempRels.begin() );
        }
    }
    while (!newRels.empty()) {
        relations.push_back( newRels.front() );
        newRels.pop_front();
    }
}

} // namespace regina

