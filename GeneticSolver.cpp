//////////////////////////////
// GeneticSolver.cpp        //
// Andrew Krepps            //
// EN.605.417 Final Project //
// 15 May 2018              //
//////////////////////////////

#include "GeneticSolver.h"

#include "Path.h"
#include "Puzzle.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

namespace gws
{
	GeneticSolver::GeneticSolver(size_t puzzleWidth, size_t puzzleHeight, size_t populationSize, size_t maxIterations)
		: m_populationSize(populationSize),
		  m_maxIterations(maxIterations),
		  m_numPuzzlePoints(Puzzle::getNumPoints(puzzleWidth, puzzleHeight)),
		  m_numPuzzleEdges(Puzzle::getNumEdges(puzzleWidth, puzzleHeight)),
		  m_numPuzzleSpaces(Puzzle::getNumSpaces(puzzleWidth, puzzleHeight))
	{
		// initialize OpenCL components
		initDeviceContextAndQueue();
		initProgramAndKernels();
		initBuffers();
	}
	
	GeneticSolver::~GeneticSolver()
	{
		// cleanup OpenCL components
		cleanup();
	}
	
	bool GeneticSolver::solvePuzzle(const Puzzle& puzzle, Path& path)
	{
		// TODO: find solution
		return false;
	}
	
	void GeneticSolver::runGenerateKernel(cl_mem outputPopulationBuffer)
	{
		// TODO: run population generation kernel
	}
	
	bool GeneticSolver::runEvaluationKernel(cl_mem inputPopulationBuffer)
	{
		// TODO: run fitness evalulation kernel
		return false;
	}
	
	void GeneticSolver::runCrossoverMutationKernel(cl_mem inputPopulationBuffer, cl_mem outputPopulationBuffer)
	{
		// TODO: run crossover/mutation kernel to generate next population
	}
	
	void GeneticSolver::initDeviceContextAndQueue()
	{
		cl_uint numPlatforms;
		cl_uint numDevices;
		
		// first, select an OpenCL platform to run on
		m_lastErrNum = clGetPlatformIDs(0, NULL, &numPlatforms);
		if (m_lastErrNum == CL_SUCCESS && numPlatforms <= 0) {
			m_lastErrNum = -1;
		}
		checkLastErr("clGetPlatformIDs");
		
		cl_platform_id* platformIDs = (cl_platform_id *)alloca(sizeof(cl_platform_id)*numPlatforms);
		
		m_lastErrNum = clGetPlatformIDs(numPlatforms, platformIDs, NULL);
		checkLastErr("clGetPlatformIDs");
		
		// iterate through the list of platforms until we find one that supports a GPU device,
		// otherwise fail with an error
		cl_uint i;
		cl_device_id* deviceIDs = NULL;
		for (i = 0; i < numPlatforms; ++i) {
			m_lastErrNum = clGetDeviceIDs(
				platformIDs[i],
				CL_DEVICE_TYPE_GPU,
				0,
				NULL,
				&numDevices);
			if (m_lastErrNum != CL_SUCCESS && m_lastErrNum != CL_DEVICE_NOT_FOUND) {
				checkLastErr("clGetDeviceIDs");
			}
			else if (numDevices > 0) {
				deviceIDs = (cl_device_id *)alloca(sizeof(cl_device_id) * numDevices);
				m_lastErrNum = clGetDeviceIDs(
					platformIDs[i],
					CL_DEVICE_TYPE_GPU,
					numDevices,
					&deviceIDs[0],
					NULL);
				checkLastErr("clGetDeviceIDs");
				
				m_deviceID = deviceIDs[0];
				break;
			}
		}
		
		// next, create an OpenCL context on the selected platform
		cl_context_properties contextProperties[] =
		{
			CL_CONTEXT_PLATFORM,
			(cl_context_properties)platformIDs[i],
			0
		};
		
		m_context = clCreateContext(
			contextProperties,
			numDevices,
			deviceIDs,
			NULL,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateContext");
		
		// create the command queue
		m_queue = clCreateCommandQueue(
			m_context,
			m_deviceID,
			0,
			&m_lastErrNum);
		checkLastErr("clCreateCommandQueue");
	}
	
	void GeneticSolver::initProgramAndKernels()
	{
		std::ifstream srcFile("GeneticSolver.cl");
		if (!srcFile.is_open()) {
			m_lastErrNum = -1;
		}
		checkLastErr("Reading GeneticSolver.cl");
		
		std::string srcProg(
			std::istreambuf_iterator<char>(srcFile),
			(std::istreambuf_iterator<char>()));
		
		const char* src = srcProg.c_str();
		size_t length = srcProg.length();
		
		// create program from source
		m_program = clCreateProgramWithSource(
			m_context,
			1,
			&src,
			&length,
			&m_lastErrNum);
		checkLastErr("clCreateProgramWithSource");
		
		// build program
		m_lastErrNum = clBuildProgram(
			m_program,
			1,
			&m_deviceID,
			NULL,
			NULL,
			NULL);
		if (m_lastErrNum != CL_SUCCESS) {
			// determine the reason for the error
			char buildLog[16384];
			clGetProgramBuildInfo(
				m_program,
				m_deviceID,
				CL_PROGRAM_BUILD_LOG,
				sizeof(buildLog),
				buildLog,
				NULL);
			
			std::cerr << "Error in OpenCL C source: " << std::endl;
			std::cerr << buildLog;
			checkLastErr("clBuildProgram");
		}
		
		// TODO: create kernels and set fixed kernel arguments
		m_generateKernel = 0;
		m_evaluateKernel = 0;
		m_crossoverMutateKernel = 0;
	}
	
	void GeneticSolver::initBuffers()
	{
		// create read-only buffers to hold constant puzzle data
		m_puzzlePointBuffer = clCreateBuffer(
			m_context,
			CL_MEM_READ_ONLY,
			sizeof(char)*m_numPuzzlePoints,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		m_puzzleEdgeBuffer = clCreateBuffer(
			m_context,
			CL_MEM_READ_ONLY,
			sizeof(char)*m_numPuzzleEdges,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		m_puzzleSpaceBuffer = clCreateBuffer(
			m_context,
			CL_MEM_READ_ONLY,
			sizeof(char)*m_numPuzzleSpaces,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		// create buffers to store populations (path solutions) and fitness results
		// (max path length is the number of puzzle points)
		m_populationBuffer1 = clCreateBuffer(
			m_context,
			CL_MEM_READ_WRITE,
			sizeof(unsigned char)*m_numPuzzlePoints*m_populationSize,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		m_populationBuffer2 = clCreateBuffer(
			m_context,
			CL_MEM_READ_WRITE,
			sizeof(unsigned char)*m_numPuzzlePoints*m_populationSize,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
		
		m_fitnessBuffer = clCreateBuffer(
			m_context,
			CL_MEM_READ_WRITE,
			sizeof(unsigned int)*m_populationSize,
			NULL,
			&m_lastErrNum);
		checkLastErr("clCreateBuffer");
	}
	
	void GeneticSolver::cleanup()
	{
		// clean up memory buffers
		if (m_puzzlePointBuffer != 0) {
			clReleaseMemObject(m_puzzlePointBuffer);
		}
		if (m_puzzleEdgeBuffer != 0) {
			clReleaseMemObject(m_puzzleEdgeBuffer);
		}
		if (m_puzzleSpaceBuffer != 0) {
			clReleaseMemObject(m_puzzleSpaceBuffer);
		}
		if (m_populationBuffer1 != 0) {
			clReleaseMemObject(m_populationBuffer1);
		}
		if (m_populationBuffer2 != 0) {
			clReleaseMemObject(m_populationBuffer2);
		}
		if (m_fitnessBuffer != 0) {
			clReleaseMemObject(m_fitnessBuffer);
		}
		
		// clean up command queue
		if (m_queue != 0) {
			clReleaseCommandQueue(m_queue);
		}
		
		// clean up kernels
		if (m_generateKernel != 0) {
			clReleaseKernel(m_generateKernel);
		}
		if (m_evaluateKernel != 0) {
			clReleaseKernel(m_evaluateKernel);
		}
		if (m_crossoverMutateKernel != 0) {
			clReleaseKernel(m_crossoverMutateKernel);
		}
		
		// clean up program
		if (m_program != 0) {
			clReleaseProgram(m_program);
		}
		
		// clean up context
		if (m_context != 0) {
			clReleaseContext(m_context);
		}
	}
	
	void GeneticSolver::checkLastErr(const std::string& name) const
	{
		if (m_lastErrNum != CL_SUCCESS) {
			std::ostringstream oss;
			oss << "ERROR: " <<  name << " (" << m_lastErrNum << ")" << std::endl;
			
			throw std::runtime_error(oss.str());
		}
	}
}