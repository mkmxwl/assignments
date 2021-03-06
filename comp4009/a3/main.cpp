#include <iostream>
#include <fstream>
#include "mpi.h"
#include "util.cpp"

#define MASTER 0

using namespace std;

int N;
bool *X; // used by p0 only for storing the entire game board

// Will display X in a 2D view using the given width and height
// If a padding of 1 is used it will ignore the outside border
void display(const bool *X, int W, int H, ostream &os, int pad) {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			os << X[(i+pad)*(W+2*pad) + j+pad];
		}
		os << endl;
	}
}

// Reads input file into X, stores true if the character is not a '0'
bool import(char *file, int N2) {
	ifstream input(file);
	if (!input.is_open()) {
		cout << "Input file: '" << file << "' could not be opened." << endl;
		return true;
	}

	char c;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			input >> c;
			X[i*N2 + j] = c != '0';
		}
	}

	return false;
}

int main(int argc, char **argv) {
	double wtime;
	ofstream output;

	MPI::Init(argc, argv);
	const int p = MPI::COMM_WORLD.Get_size();
	const int id = MPI::COMM_WORLD.Get_rank();

	if (argc != 7) {
		cout << "Usage: life <input> <N> <p1> <p2> <k> <m>" << endl;
		return 1;
	}

	// Initializing all variables from CLI arguments
	N = stoi(argv[2]);
	int p1 = stoi(argv[3]);
	int p2 = stoi(argv[4]);
	int k = stoi(argv[5]);
	int m = stoi(argv[6]);

	if (p != p1*p2) {
		cout << "The number of processors must be equal to p1 x p2" << endl;
		return 2;
	}

	// Y and Z are the sections for working on the cells
	const int N2 = N+2; // side length of original grid + 2 for padding
	const int PW = N/p1; // number of processors that divide the data vertically
	const int PH = N/p2; // ^^ horizontally
	const int PW2 = PW + 2;
	const int PH2 = PH + 2;
	const int NP = PW * PH; // total size of data each processor is given
	const int NP2 = PW2 * PH2; // including the padding values

	bool *Xt;
	bool Yt[NP];

	// Master processor reads input file into an array
	// Prints information about the run to standard out
	// Prepares the data into quadrants for the other processors.
	if (id == MASTER) {
		X = (bool*) calloc(N2*N2, sizeof(bool));
		bool err = import(argv[1], N2);
		if (err) MPI::COMM_WORLD.Abort(1);
		if (m) output.open("output.txt");

		cout << "MPI Settings: p = " << p << ", p1 = " << p1 << ", p2 = " << p2 << endl;
		cout << "Game of Life: N = " << N << ", k = " << k << ", m = " << m << endl;
		#ifdef DEBUG
		cout << "Generation: 0" << endl;
		display(X, N, N, cout, 1);
		#endif

		wtime = MPI::Wtime();

		bool Xs[N*N];
		remove_pad(X, Xs, N, N);
		Xt = (bool*) malloc(N*N * sizeof(bool));
		convert(Xs, Xt, N, p1, p2, true); // try to reuse this function
		// display(Xt, NP, p, cout, 0);
		X = (bool*) realloc(X, N*N * sizeof(bool));
	}

	bool Y[NP2] = { 0 };
	bool Z[NP2] = { 0 };
	bool *curPtr = Y;
	bool *setPtr = Z;

	// Sends quandrant to their respective processor
	MPI::COMM_WORLD.Scatter(Xt, NP, MPI::BOOL, curPtr, NP, MPI::BOOL, MASTER);
	// Moves received data within its array so it has a 1 cell border
	add_pad(curPtr, PW, PH);

	// Create send and receive buffers, whose sizes depend on the perimeter
	const int PRMT = perimeter(PW, PH);
	bool *sbuf = (bool*) calloc(PRMT-2, sizeof(bool));
	bool *rbuf = (bool*) calloc(PRMT+4, sizeof(bool));
	int offs[9];
	int counts[p] = { 0 };
	int sdispls[p] = { 0 };
	int rdispls[p] = { 0 };

	// Function calls to set values of counts which are the same for sending and receiving
	// and the displacements of the alltoallv
	make_counts(counts, id, p1, p2, N);
	
	get_offsets(offs, PW, PH);
	make_displs(sdispls, offs, id, p1, p2);

	get_offsets_r(offs, PW2, PH2);
	make_displs(rdispls, offs, id, p1, p2);

	// The generation loop
	for (int gen = 1; gen <= k; gen++) {
		// Fills the send buffer with the processors local border
		create_halo(curPtr+PW2+1, sbuf, PW2, PW, PH);
		MPI::COMM_WORLD.Alltoallv(
			sbuf, counts, sdispls, MPI::BOOL,
			rbuf, counts, rdispls, MPI::BOOL
		);
		// Takes the received data from the other processors and writes it to the local array
		// on the very outside edges, needed to perform the evolution
		overwrite_halo(curPtr, rbuf, counts, rdispls, id, p1, p2, PW2, PH2);

		// Evolve all of the local cells, (ones that have neighbours on all sides)
		for (int i = 1; i <= PH; i++) {
			for (int j = 1; j <= PW; j++)
				evolve(curPtr, setPtr, i*PW2 + j, PW2);
		}

		// If the current generation is an m-th, Master processor will gather data quadrants
		// from all the processors and write it out to a file
		if (m && gen % m == 0) {
			// Need to remove the padding, so we only the send the local data
			remove_pad(setPtr, Yt, PW, PH);
			MPI::COMM_WORLD.Gather(
				Yt, NP, MPI::BOOL,
				Xt, NP, MPI::BOOL, MASTER
			);
			if (id == MASTER) {
				convert(X, Xt, N, p1, p2, false);
				#ifdef DEBUG
				cout << "---------------" << endl;
				cout << "Generation: " << gen << endl;
				display(X, N, N, cout, 0);
				#endif
				display(X, N, N, output, 0);
			}
		}

		// Switch cur and set pointers so in the next generation we can evolve without conflict
		swap(curPtr, setPtr);
	}

	// Deallocate memory and close file handles
	free(sbuf);
	free(rbuf);
	if (id == MASTER) {
		free(X);
		free(Xt);
		if (output.is_open())
			output.close();
		wtime = MPI::Wtime() - wtime;
		cout << "Time Elapsed: " << wtime << " seconds." << endl;
	}

	MPI::Finalize();
	return 0;
}