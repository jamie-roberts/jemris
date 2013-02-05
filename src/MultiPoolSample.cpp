#include "MultiPoolSample.h"
#include "BinaryContext.h"

MultiPoolSample::MultiPoolSample (std::string fname) {

	Prepare(fname);
	CropEnumerate();

} 


MultiPoolSample::MultiPoolSample (long size) {

	Prepare();

} 


IO::Status MultiPoolSample::Populate (string fname) {

	// Doing 
	Sample::Populate (fname);

	BinaryContext bc;
	DataInfo      di;
	IO::Status    ios;

	bc.Initialize (fname, IO::IN);
	if (bc.Status() != IO::OK)
		return bc.Status();

	di = bc.GetInfo (std::string("/sample/helper"));
	if (bc.Status() != IO::OK)
		return bc.Status();

	if (di.NDim() != 2)
		return IO::UNMATCHED_DIMENSIONS;

	m_no_spin_compartments = di.Dims()[0];

	// Helper in world is used for storing the global exchange rates
	CreateHelper(m_no_spin_compartments*m_no_spin_compartments);

	bc.ReadData (m_helper);

	return IO::OK;

}



void MultiPoolSample::Prepare (std::string fname) {

	Sample::Prepare(fname);

}

/**********************************************************/
void MultiPoolSample::CropEnumerate () {
	
	int  nsize = 0;
	long osize = m_ensemble.NSpins();
 	int nprops = m_ensemble.NProps();

	double* tmp = (double*) malloc (osize * nprops * sizeof(double));
	memcpy (tmp, m_ensemble.Data(), osize * nprops * sizeof(double));

	for (int i = 0; i < osize; i++) {

		int nonzero = 0;
		for (int j = 0; j < m_no_spin_compartments; j++)
			if (tmp[i* nprops + j*m_no_spin_compartments + M0] > 0)
				nonzero++;
		
		if (nonzero > 0)
			nsize++;

	}

	m_ensemble.ClearData();
	m_ensemble.Init(nsize);

	int n = 0;
	for (int i = 0; i < osize; i++) {

		int nonzero = 0;
		for (int j = 0; j < m_no_spin_compartments; j++)
			if (tmp[i* nprops + j*m_no_spin_compartments + M0] > 0)
				nonzero++;


		if (nonzero > 0) {

			long npos = n * nprops;

			memcpy (&m_ensemble[npos], &tmp[i * nprops] , nprops * sizeof(double));
			m_ensemble[npos + nprops - 1] = n;
			m_spin_state.push_back(0);
			n++;

		}

	}

	free (tmp);

}
