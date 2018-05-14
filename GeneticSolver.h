//////////////////////////////
// GeneticSolver.h          //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#ifndef gws_GeneticSolver_h
#define gws_GeneticSolver_h

#include "Solver.h"

#ifdef __APPLE__
	#include <OpenCL/cl.h>
#else
	#include <CL/cl.h>
#endif

#include <string>

namespace gws
{
	class Path;
	class Puzzle;
	
	///////////////////////////////////////////////////////////////////////////
	/// \class GeneticSolver
	/// \brief Puzzle solver that executes a genetic algorithm on the GPU
	///////////////////////////////////////////////////////////////////////////
	class GeneticSolver : public Solver
	{
	public:
		GeneticSolver(size_t puzzleWidth, size_t puzzleHeight, size_t populationSize, size_t maxIterations, unsigned int seed);
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Virtual destructor
		///////////////////////////////////////////////////////////////////////
		virtual ~GeneticSolver();
		
		// prevent creating copies of the solver
		GeneticSolver(const GeneticSolver& other) = delete;
		GeneticSolver& operator=(const GeneticSolver* other) = delete;
		
		///////////////////////////////////////////////////////////////////////
		/// \copydoc Solver::solvePuzzle()
		///////////////////////////////////////////////////////////////////////
		virtual bool solvePuzzle(const Puzzle& puzzle, Path& path);
		
		///////////////////////////////////////////////////////////////////////
		/// \brief Get the number of iterations used to solve the puzzle
		/// 
		/// \returns the number of iterations
		///////////////////////////////////////////////////////////////////////
		size_t getNumIterations() const;
		
	private:
		size_t m_puzzleWidth;
		size_t m_puzzleHeight;
		size_t m_numPuzzlePoints;
		size_t m_numPuzzleEdges;
		size_t m_numPuzzleSpaces;
		
		size_t m_populationSize;
		size_t m_maxIterations;
		unsigned int m_seed;
		
		size_t m_numIterations;
		
		cl_int m_lastErrNum;
		
		cl_device_id m_deviceID;
		cl_context m_context;
		cl_command_queue m_queue;
		
		cl_program m_program;
		cl_kernel m_evaluateKernel;
		
		cl_mem m_puzzlePointBuffer;
		cl_mem m_puzzleEdgeBuffer;
		cl_mem m_puzzleSpaceBuffer;
		
		cl_mem m_populationBuffer;
		cl_mem m_fitnessBuffer;
		cl_mem m_startPointBuffer;
		cl_mem m_pathsBuffer;
		
		int calcMaxFitness(const Puzzle& puzzle) const;
		
		void fillPath(const char* paths, const unsigned int* startPoints, size_t member, size_t maxLength, Path& path) const;
		
		void initDeviceContextAndQueue();
		void initBuffers();
		void initProgramAndKernels();
		
		void transferPuzzleData(const Puzzle& puzzle);
		
		void runEvaluationKernel(const unsigned char* population, int* fitness, unsigned int* startPoints, char* paths);
		
		void cleanup();
		
		void checkLastErr(const std::string& name) const;
	};
}

#endif