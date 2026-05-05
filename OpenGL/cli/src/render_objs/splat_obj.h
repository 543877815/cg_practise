//#pragma once
//
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <unordered_map>
//#include <vector>
//#include <memory>
//#include <glm/glm.hpp>
//#include "../vertexbuffer.h"
//#include "./common.h"
//#include "../aabb.h"
//
//enum SORT_ORDER {
//	DESCENDING
//};
//struct ImguiParameters {
//
//};
//
//
//struct PlyProperty {
//	std::string type;
//	std::string name;
//};
//
//struct PackPlyHeader {
//	std::string format;
//	int numVertices = 0;
//	int numFaces = 0;
//	int numChunks = 0;
//	std::unordered_map<std::string, int> element2offest;
//	std::unordered_map<std::string, std::vector<PlyProperty>> element2Properties;
//};
//
//struct PlyHeader {
//	std::string format;
//	int numVertices = 0;
//	int numFaces = 0;
//	int vertex_offset = 0;
//	std::vector<PlyProperty> vertexProperties;
//	std::vector<PlyProperty> faceProperties;
//	std::vector<std::string> verticePropertiesOrder;
//	std::unordered_map <std::string, std::pair<size_t, size_t>> verticePropertiesOffset;
//
//	void UpdateVerticesOffset(std::string property, std::pair<size_t, size_t>& start_end) {
//		if (verticePropertiesOffset.find(property) == verticePropertiesOffset.end()) {
//			verticePropertiesOrder.emplace_back(property);
//			verticePropertiesOffset[property] = { start_end.first, start_end.second - start_end.first };
//		}
//		else {
//			verticePropertiesOffset[property].second = start_end.second - verticePropertiesOffset[property].first;
//		}
//	}
//};
//
//struct PlyVertexStorage {
//	glm::vec3 position;
//	glm::vec3 normal{ 0.0f, 0.0f, 0.0f };
//	float shs[3];
//	float opacity;
//	glm::vec3 scale;
//	glm::vec4 rotation;
//};
//
//struct PlyVertex3 {
//	glm::vec4 position;
//	glm::vec3 normal{ 0.0f, 0.0f, 0.0f };
//	float shs[3];
//	float opacity;
//	glm::vec3 scale;
//	glm::vec4 rotation;
//};
//
//struct PlyVertex2 {
//	glm::vec3 position;
//	float shs[27];
//	float opacity;
//	glm::vec4 rotation;
//	glm::vec3 scale;
//};
//
//struct PlyVertex1 {
//	glm::vec3 position;
//	float shs[12];
//	float opacity;
//	glm::vec4 rotation;
//	glm::vec3 scale;
//};
//
//struct PlyVertex0 {
//	glm::vec3 position;
//	float shs[3];
//	float opacity;
//	glm::vec4 rotation;
//	glm::vec3 scale;
//};
//
//struct SplatVertex {
//	glm::vec3 position;
//	glm::vec3 scale;
//	uint8_t shs[4];  // 0.5 * SH_C0 * v["f_dc"]
//	uint8_t rotation[4];
//};
//
//struct SplatBuffer {
//	uint8_t data[32];
//};
//
//void GetPlyHeader(std::ifstream& file, PlyHeader& header);
//void GetPlyHeader(std::ifstream& file, PackPlyHeader& header);
//
//enum MODEL_TYPE
//{
//	SPLAT, PLY, PACKED_PLY
//};
//
//class SplatRenderer {
//public:
//	void I_RunSortShader(glm::mat4& modelViewProjMatrix, const glm::vec2& projParams, const ImguiParameters& imguiParams);
//	std::vector<uint32_t>& GetTexData() { return m_texdata; };
//	std::shared_ptr<AABB>& GetAABB() { return m_aabb; };
//	virtual size_t GetVertexCount() { return m_vertexCount; };
//	virtual std::shared_ptr<Shader>& GetRenderProgram() { return m_renderProgram; }
//
//#ifndef USE_GPU_SORT   
//	std::vector<uint32_t>& GetDepthIndex() { return m_depthIndex; };
//	virtual void I_RunSortUpdateDepth(glm::mat4& modelViewProjMatrix, bool useRadixSort = true, SORT_ORDER sortOrder = DESCENDING) = 0;
//#endif
//
//protected:
//	uint32_t m_vertexCount, m_vertexLength;
//	size_t m_texwidth, m_texheight;
//	MODEL_TYPE m_type;
//	std::vector<uint32_t> m_texdata;
//	std::shared_ptr<Shader> m_renderProgram = nullptr;
//	std::shared_ptr<Texture> m_gaussian_texture = nullptr;
//	std::shared_ptr<VertexArrayObject> m_renderBufferVAO;
//	std::shared_ptr<BufferObject> m_rectangleBuffer;
//	std::shared_ptr<AABB> m_aabb;
//	std::vector<size_t> m_indices;
//
//	virtual void LoadVertices(std::ifstream& file) = 0;
//	virtual void SetupShader(const char* vertexPath, const char* fragmentPath, const char* presortPath = nullptr, const char* sortPath = nullptr);
//	virtual void SetupVertexBuffer();
//	virtual void GenerateTex();
//	template <typename T> void GenerateAABB(std::vector<T>& m_vertices);
//	template <typename T> void SortIndices(std::vector<T>& vertices);
//#ifdef USE_GPU_SORT
//	std::shared_ptr<ComputeShader> m_presortProgram = nullptr;
//	std::shared_ptr<ComputeShader> m_sortProgram = nullptr;
//	std::shared_ptr<BufferObject> m_keyBuffer;
//	std::shared_ptr<BufferObject> m_keyBuffer2;
//	std::shared_ptr<BufferObject> m_histogramBuffer;
//	std::shared_ptr<BufferObject> m_valBuffer;
//	std::shared_ptr<BufferObject> m_valBuffer2;
//	std::shared_ptr<BufferObject> m_posBuffer;
//	std::shared_ptr<BufferObject> m_atomicCounterBuffer;
//	std::shared_ptr<BufferObject> m_presortUniformBuffer;
//	std::shared_ptr<BufferObject> m_sortUniformBuffer;
//	std::vector<glm::vec4> m_posVec;
//	std::vector<uint32_t> m_indexVec;
//	std::vector<uint32_t> m_depthVec;
//	uint32_t m_atomicCounterVec = 0;
//	uint32_t g_numBlocksPerWorkgroup = 1024;
//	void SetupComputeBuffer();
//	struct sortUniform {
//		uint32_t vertexCount;
//		uint32_t sortOrder;
//	} m_sortUniform;
//#else
//	std::vector<uint32_t> m_depthIndex;
//	std::vector<std::pair<uint32_t, float>> m_Index2depth;
//	template <typename T> void RunSortUpdateDepth(std::vector<T>& m_vertices, glm::mat4& modelViewProjMatrix, bool useRadixSort = true, SORT_ORDER sortOrder = DESCENDING);
//	GLuint m_depthIndexVBO = 0;
//	std::shared_ptr<BufferObject> m_depthIndexBuffer;
//	std::vector<int32_t> m_sizeList;
//	std::vector<uint32_t> m_counts;
//	std::vector<uint32_t> m_starts;
//	template <typename T> void RadixSort(std::vector<T>& m_vertices, glm::mat4& modelViewProjMatrix, SORT_ORDER sortOrder);
//	template <typename T> void QuickSort(std::vector<T>& m_vertices, glm::mat4& modelViewProjMatrix, SORT_ORDER sortOrder);
//#endif // !USE_GPU_SORT
//};
//
//class PlyModel :public SplatRenderer {
//public:
//	struct vertexUniform {
//		int u_texture;
//		glm::mat4 model;
//		glm::mat4 view;
//		glm::mat4 projection;
//		glm::vec2 focal;
//		glm::vec2 viewport;
//		glm::vec3 cam_pos;
//		glm::vec2 tan_fov;
//		glm::vec2 projParams;
//		int sh_deg;
//		int showGaussian;
//		int showHotspots;
//	};
//
//	PlyModel() {};
//	PlyModel(std::ifstream& file, PlyHeader& m_header, const char* vertexPath, const char* fragmentPath);
//	virtual void Render(const vertexUniform& uniform, const ImguiParameters& imguiParams, bool isDirty);
//#ifndef USE_GPU_SORT
//	virtual void I_RunSortUpdateDepth(glm::mat4& modelViewProjMatrix, bool useRadixSort, SORT_ORDER sortOrder = DESCENDING) override;
//#endif
//
//private:
//	PlyHeader m_header;
//
//protected:
//	std::vector<PlyVertex3> m_vertices;
//	virtual void GenerateTexData();
//	virtual void GetSigmaFloat32(glm::vec4& rotation, glm::vec3& scale, std::vector<float>& sigmas);
//	virtual void LoadVertices(std::ifstream& file) override;
//	virtual void GenerateAABB() { SplatRenderer::GenerateAABB<PlyVertex3>(m_vertices); }
//};
//
//class PackedPlyModel :public PlyModel {
//public:
//	PackedPlyModel(std::ifstream& file, PackPlyHeader& m_header, const char* vertexPath, const char* fragmentPath);
//
//private:
//	struct ChunkData {
//		float min_x, min_y, min_z;
//		float max_x, max_y, max_z;
//		float min_scale_x, min_scale_y, min_scale_z;
//		float max_scale_x, max_scale_y, max_scale_z;
//	};
//
//	struct VertexData {
//		uint32_t position;
//		uint32_t rotation;
//		uint32_t scale;
//		uint32_t color;
//	};
//
//	PackPlyHeader m_header;
//	virtual void LoadVertices(std::ifstream& file) override;
//
//};
//
//class SplatModel : public SplatRenderer {
//public:
//	struct vertexUniform {
//		int u_texture;
//		glm::mat4 model;
//		glm::mat4 view;
//		glm::mat4 projection;
//		glm::vec2 focal;
//		glm::vec2 viewport;
//		glm::vec2& projParams;
//		bool showGaussian = 0;
//	};
//	std::vector<SplatVertex> m_vertices;
//	SplatModel(const std::string& filename, const char* vertexPath, const char* fragmentPath);
//	void Render(const vertexUniform& uniform, const ImguiParameters& imguiParams, bool isDirty);
//#ifndef USE_GPU_SORT
//	void I_RunSortUpdateDepth(glm::mat4& modelViewProjMatrix, bool useRadixSort, SORT_ORDER sortOrder = DESCENDING) override;
//#endif
//
//private:
//	void LoadVertices(std::ifstream& file) override;
//	void GetSigmaHalf2x16(SplatVertex& vertexBuffer, std::vector<uint32_t>& sigmasHalf2x16);
//	void GenerateAABB() { SplatRenderer::GenerateAABB<SplatVertex>(m_vertices); }
//};
//
////class VertexArrayObject
////{
////public:
////	VertexArrayObject();
////	~VertexArrayObject();
////
////	void Bind() const;
////	void Unbind() const;
////
////	void SetAttribBuffer(int loc, std::shared_ptr<BufferObject>& attribBufferIn, bool instance = false);
////	void SetElementBuffer(std::shared_ptr<BufferObject>& elementBufferIn);
////	std::shared_ptr<BufferObject> GetElementBuffer() const { return elementBuffer; }
////	void DrawElements(int mode) const;
////
////protected:
////	uint32_t obj;
////	std::vector<std::shared_ptr<BufferObject>> attribBufferVec;
////	std::shared_ptr<BufferObject> elementBuffer;
////};
////VertexArrayObject::VertexArrayObject()
////{
////	glGenVertexArrays(1, &obj);
////}
////
////VertexArrayObject::~VertexArrayObject()
////{
////	glDeleteVertexArrays(1, &obj);
////}
////
////void VertexArrayObject::Bind() const
////{
////	glBindVertexArray(obj);
////}
////
////void VertexArrayObject::Unbind() const
////{
////	glBindVertexArray(0);
////}
////
////void VertexArrayObject::SetAttribBuffer(int loc, std::shared_ptr<BufferObject>& attribBuffer, bool instance)
////{
////
////	assert(attribBuffer->target == GL_ARRAY_BUFFER);
////	Bind();
////	attribBuffer->Bind();
////	glVertexAttribPointer(loc, attribBuffer->elementSize, attribBuffer->type, attribBuffer->normalized,
////		attribBuffer->offset * sizeof(attribBuffer->type), (void*)(attribBuffer->stride * sizeof(attribBuffer->type)));
////	glEnableVertexAttribArray(loc);
////	if (instance) {
////		glVertexAttribDivisor(loc, 1);
////	}
////	attribBuffer->Unbind();
////	attribBufferVec.push_back(attribBuffer);
////	Unbind();
////}
////
////void VertexArrayObject::SetElementBuffer(std::shared_ptr<BufferObject>& elementBufferIn)
////{
////	assert(elementBufferIn->target == GL_ELEMENT_ARRAY_BUFFER);
////	elementBuffer = elementBufferIn;
////
////	Bind();
////	elementBufferIn->Bind();
////	Unbind();
////}
////
////void VertexArrayObject::DrawElements(int mode) const
////{
////	Bind();
////	glDrawElements((GLenum)mode, elementBuffer->numElements, GL_UNSIGNED_INT, nullptr);
////	Unbind();
////}