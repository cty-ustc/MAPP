#ifndef MODSPEC_ELEMENT_H
#define MODSPEC_ELEMENT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include "untyped.h"
#include "sacado.h"

using namespace std;

class ModSpec_Element_without_fqei { // this is a pure virtual base class since fqei is a pure virtual function
	protected:
		string name;

		vector<string> parm_names; // single-word string used in, eg, SPICE files
		vector<string> parm_descriptions; // longer description of each parameter
		vector<string> parm_units; // single-word string used to denote the units
		vector<untyped> parm_vals; // used to store current values of parms
		vector<untyped> parm_defaultvals; //  default values of parameters

		vector<string> node_names;
		string refnode_name;

		vector<string> io_types;
		vector<string> io_nodenames;
		vector<string> explicit_output_names;
		vector<string> internal_unk_names;
		vector<string> implicit_equation_names;
		vector<string> u_names;

		// derived from the above (these are set up in the constructor)
		vector<string> io_names;
		vector<string> otherio_names;

		void setup_ios_otherios(){
			// set up a list of non-refnode node names
			vector<string> nonref_nodenames = node_names;
			int nnodes = node_names.size(); // of which one is the reference node
			for (int i=0; i<nnodes; i++) {
				if (0 == strcmp(refnode_name.c_str(), node_names[i].c_str())) {
					nonref_nodenames.erase(nonref_nodenames.begin() + i);
					break;
				}
			}

			// set up io_names from node_names and refnode_name
			// stringstream sstrV, sstrI;
			for (int i=0; i<nnodes-1; i++) {
				string nname = nonref_nodenames[i];
				/*
				sprintf((char*) sstrV, "v%s%s", nname.c_str(), refnode_name.c_str());
				sprintf(sstrI, "i%s%s", nname.c_str(), refnode_name.c_str());
				*/
				io_names.push_back("v" + nname + refnode_name);
			}
			for (int i=0; i<nnodes-1; i++) {
				string nname = nonref_nodenames[i];
				io_names.push_back("i" + nname + refnode_name);
			}

			// set up otherio_names by excluding explicit_output_names from io_names
			int neIOs = explicit_output_names.size();
			otherio_names = io_names;
			vector<string>::iterator vsIter; // std::find returns this type
			for (int i=0; i<neIOs; i++) {
				vsIter = find(otherio_names.begin(), otherio_names.end(), explicit_output_names[i]);
				if (vsIter != otherio_names.end()) {
					// found; delete it
					otherio_names.erase(vsIter);
				} else {
					fprintf(stderr, "explicit output %s not found in IOnames\n", explicit_output_names[i].c_str());
				}
			}
		}; 
	public:
		ModSpec_Element_without_fqei(){ // constructor
			// name = nm;
		};
		~ModSpec_Element_without_fqei(){};
		
		// no operators? should at least define
		// - copy constructor
		// - operator=

		virtual vector<string> parmnames() {
			return parm_names;
		};
		virtual vector<string> parmdescriptions() {
			return parm_descriptions;
		};
		virtual vector<string> parmunits() {
			return parm_units;
		};
		virtual vector<untyped> parmdefaults() {// vector<double> a temporary hack, need untyped
			return parm_defaultvals;
		};
		virtual vector<untyped> getparms() {// vector<double> a temporary hack, need untyped
			return parm_vals;
		}; 

		virtual void setparms(vector<untyped>& a) { // simplest incarnation, needs updates
			parm_vals = a;// vector<double> a temporary hack, need untyped
		}

		virtual vector<string> NIL_NodeNames() {
			return node_names;
		}; 
		virtual string NIL_RefNodeName() {
			return refnode_name;
		};
		virtual vector<string> IOnames() { 
			return io_names;
		};
		virtual vector<string> NIL_IOtypes() {
			return io_types;
		};
		virtual vector<string> NIL_IONodeNames() {
			return io_nodenames;
		};
		virtual vector<string> ExplicitOutputNames() {
			return explicit_output_names;
		};
		virtual vector<string> OtherIOnames() { 
			return otherio_names;
		};
		virtual vector<string> InternalUnkNames() {
			return internal_unk_names;
		};
		virtual vector<string> ImplicitEquationNames() {
			return implicit_equation_names;
		};
		virtual vector<string> Unames() {
			return u_names;
		};

		/*
		template <typename TOUT, typename TX, typename TY, typename TU>
		  vector<TOUT> fe(vector<TX>& vecX, vector<TY>& vecY, vector<TU>& vecU) {
			return fqei<TOUT,TX,TY,TU>(vecX, vecY, vecU, 'e', 'f');
		  };

		template <typename TOUT, typename TX, typename TY, typename TU>
		  vector<TOUT> fi(vector<TX>& vecX, vector<TY>& vecY, vector<TU>& vecU) {
			return fqei<TOUT,TX,TY,TU>(vecX, vecY, vecU, 'i', 'f');
		  };

		template <typename TOUT, typename TX, typename TY, typename TU>
		  vector<TOUT> qe(vector<TX>& vecX, vector<TY>& vecY) {
			vector<TU> oof;
			return fqei<TOUT,TX,TY,TU>(vecX, vecY, oof, 'e', 'q');
		  };

		template <typename TOUT, typename TX, typename TY, typename TU>
		  vector<TOUT> qi(vector<TX>& vecX, vector<TY>& vecY) {
			vector<TU> oof;
			return fqei<TOUT,TX,TY,TU>(vecX, vecY, oof, 'i', 'q');
		  };
	protected:
		template <typename TOUT, typename TX, typename TY, typename TU>
		  virtual vector<TOUT> fqei(vector<TX>& vecX, vector<TY>& vecY, vector<TU>& vecU, char eORi, char fORq) = 0;
			// this is a pure virtual function, it must be overloaded by the derived class
		*/
};

// the main ModSpec_Element class: it has fqei, but no derivatives
class ModSpec_Element: public ModSpec_Element_without_fqei {
	public:
		vector<double> fe(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) {
			return fqei(vecX, vecY, vecU, 'e', 'f');
		};

		vector<double> fi(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) {
			return fqei(vecX, vecY, vecU, 'i', 'f');
		};

		vector<double> qe(vector<double>& vecX, vector<double>& vecY) {
			vector<double> oof;
			return fqei(vecX, vecY, oof, 'e', 'q');
		};

		vector<double> qi(vector<double>& vecX, vector<double>& vecY) {
			vector<double> oof;
			return fqei(vecX, vecY, oof, 'i', 'q');
		};
	protected:
		virtual vector<double> fqei(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU, char eORi, char fORq) = 0;
			// this is a pure virtual function, it must be overloaded by the derived class

};

class ModSpec_Element_with_Jacobians: public ModSpec_Element { // fqei still virtual here, plus a bunch of other virtual functions
	protected:

	public:
		// jacobian stamps
		// ddX functions
		virtual spMatrix dfe_dX_stamp(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		virtual spMatrix dfi_dX_stamp(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		virtual spMatrix dqe_dX_stamp(vector<double>& vecX, vector<double>& vecY) = 0;
		virtual spMatrix dqi_dX_stamp(vector<double>& vecX, vector<double>& vecY) = 0;

		// ddY functions
		virtual spMatrix dfe_dY_stamp(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		virtual spMatrix dfi_dY_stamp(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		virtual spMatrix dqe_dY_stamp(vector<double>& vecX, vector<double>& vecY) = 0;
		virtual spMatrix dqi_dY_stamp(vector<double>& vecX, vector<double>& vecY) = 0;
		
		// ddU functions
		virtual spMatrix dfe_dU_stamp(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		virtual spMatrix dfi_dU_stamp(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		
		// numerical jacobians
		// ddX functions
		virtual spMatrix dfe_dX(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		virtual spMatrix dfi_dX(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		virtual spMatrix dqe_dX(vector<double>& vecX, vector<double>& vecY) = 0;
		virtual spMatrix dqi_dX(vector<double>& vecX, vector<double>& vecY) = 0;

		// ddY functions
		virtual spMatrix dfe_dY(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		virtual spMatrix dfi_dY(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		virtual spMatrix dqe_dY(vector<double>& vecX, vector<double>& vecY) = 0;
		virtual spMatrix dqi_dY(vector<double>& vecX, vector<double>& vecY) = 0;
		
		// ddU functions
		virtual spMatrix dfe_dU(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
		virtual spMatrix dfi_dU(vector<double>& vecX, vector<double>& vecY, vector<double>& vecU) = 0;
};

/*
class ModSpec_Element: public ModSpec_Element_irredundant {
	public:
}
*/

#endif // MODSPEC_ELEMENT_H
