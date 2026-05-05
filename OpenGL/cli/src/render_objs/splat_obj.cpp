//#include "splat_obj.h"
//
// uint32_t unionFloatToUint32(float f) {
//FloatIntUnion fiu;
//fiu.f = f;
//return fiu.ui;
//}
//
//int32_t unionFloatToInt32(float f) {
//	FloatIntUnion fiu;
//	fiu.f = f;
//	return fiu.i;
//}
//
//
//float unionUint32ToFloat(uint32_t ui) {
//	FloatIntUnion fiu;
//	fiu.ui = ui;
//	return fiu.f;
//}
//
//uint32_t packHalf2x16(float x, float y) {
//	return (floatToHalf(x) | (floatToHalf(y) << 16));
//}
//
//uint32_t floatToHalf(float f) {
//	FloatIntUnion fiu;
//	fiu.f = f;
//	uint32_t i = fiu.ui;
//	uint32_t sign = (i >> 31) & 0x0001;
//	uint32_t exp = (i >> 23) & 0x00ff;
//	uint32_t frac = i & 0x007fffff;
//
//	int newExp;
//	if (exp == 0) {
//		newExp = 0;
//	}
//	else if (exp < 113) {
//		newExp = 0;
//		frac |= 0x00800000;
//		frac = frac >> (113 - exp);
//		if (frac & 0x01000000) {
//			newExp = 1;
//			frac = 0;
//		}
//	}
//	else if (exp < 142) {
//		newExp = exp - 112;
//	}
//	else {
//		newExp = 31;
//		frac = 0;
//	}
//	return (sign << 15) | (newExp << 10) | (frac >> 13);
//}
// 
// template<typename T>
//void GetSigma(std::vector<T>& scale, std::vector<T>& rot, std::vector<T>& sigma)
//{
//    std::vector<T> M{
//        1.0 - 2.0 * (rot[2] * rot[2] + rot[3] * rot[3]),
//        2.0 * (rot[1] * rot[2] + rot[0] * rot[3]),
//        2.0 * (rot[1] * rot[3] - rot[0] * rot[2]),
//
//        2.0 * (rot[1] * rot[2] - rot[0] * rot[3]),
//        1.0 - 2.0 * (rot[1] * rot[1] + rot[3] * rot[3]),
//        2.0 * (rot[2] * rot[3] + rot[0] * rot[1]),
//
//        2.0 * (rot[1] * rot[3] + rot[0] * rot[2]),
//        2.0 * (rot[2] * rot[3] - rot[0] * rot[1]),
//        1.0 - 2.0 * (rot[1] * rot[1] + rot[2] * rot[2]),
//    };
//
//    for (size_t i = 0; i < M.size(); i++) {
//        M[i] *= scale[std::floor(i / 3)];
//    }
//
//    sigma[0] = M[0] * M[0] + M[3] * M[3] + M[6] * M[6];
//    sigma[1] = M[0] * M[1] + M[3] * M[4] + M[6] * M[7];
//    sigma[2] = M[0] * M[2] + M[3] * M[5] + M[6] * M[8];
//    sigma[3] = M[1] * M[1] + M[4] * M[4] + M[7] * M[7];
//    sigma[4] = M[1] * M[2] + M[4] * M[5] + M[7] * M[8];
//    sigma[5] = M[2] * M[2] + M[5] * M[5] + M[8] * M[8];
//}


//static const float SH_C0 = 0.28209479177387814f;
//static const float SH_C1 = 0.4886025119029199f;
//static const float SH_C2[] = {
//	1.0925484305920792f,
//	-1.0925484305920792f,
//	0.31539156525252005f,
//	-1.0925484305920792f,
//	0.5462742152960396f
//};
//static const float SH_C3[] = {
//	-0.5900435899266435f,
//	2.890611442640554f,
//	-0.4570457994644658f,
//	0.3731763325901154f,
//	-0.4570457994644658f,
//	1.445305721320277f,
//	-0.5900435899266435f
//};
//
//void GetPlyHeader(std::ifstream& file, PackPlyHeader& header) {
//	std::string line;
//	bool headerEnd = false;
//	std::string currentElement;
//	header.element2offest["chunk"] = 0;
//	header.element2offest["vertex"] = 0;
//
//	while (std::getline(file, line)) {
//		std::istringstream iss(line);
//		std::string token;
//		std::cout << line << std::endl;
//		iss >> token;
//		if (token == "ply") {
//			// PLY format indicator
//		}
//		else if (token == "comment") {
//			iss >> token >> token >> token;
//		}
//		else if (token == "element") {
//			iss >> token;
//			if (token == (currentElement = "chunk")) {
//				iss >> header.numChunks;
//			}
//			else if (token == (currentElement = "vertex")) {
//				iss >> header.numVertices;
//			}
//			else if (token == (currentElement = "face")) {
//				iss >> header.numFaces;
//			}
//		}
//		else if (token == "property") {
//			PlyProperty property;
//			iss >> property.type >> property.name;
//			header.element2Properties[currentElement].emplace_back(property);
//			if (property.type == "char" || property.type == "int8")
//				header.element2offest[currentElement] += 1;
//			else if (property.type == "uchar" || property.type == "uint8")
//				header.element2offest[currentElement] += 1;
//			else if (property.type == "short" || property.type == "int16")
//				header.element2offest[currentElement] += 2;
//			else if (property.type == "ushort" || property.type == "uint16")
//				header.element2offest[currentElement] += 2;
//			else if (property.type == "int" || property.type == "int32")
//				header.element2offest[currentElement] += 4;
//			else if (property.type == "uint" || property.type == "uint32")
//				header.element2offest[currentElement] += 4;
//			else if (property.type == "float" || property.type == "float32")
//				header.element2offest[currentElement] += 4;
//			else if (property.type == "double" || property.type == "float64")
//				header.element2offest[currentElement] += 8;
//			else
//				std::cout << "Unsupported type " << property.type.c_str() << " for property" << property.name << std::endl;
//		}
//		else if (token == "end_header") {
//			headerEnd = true;
//			break;
//		}
//	}
//	if (!headerEnd) {
//		throw std::runtime_error("Could not find end of header");
//	}
//}
//
//void GetPlyHeader(std::ifstream& file, PlyHeader& header) {
//	std::string line;
//	bool headerEnd = false;
//	while (std::getline(file, line)) {
//		std::istringstream iss(line);
//		std::string token;
//		std::cout << line << std::endl;
//		iss >> token;
//		if (token == "ply") {
//			// PLY format indicator
//		}
//		else if (token == "format") {
//			iss >> header.format;
//		}
//		else if (token == "element") {
//			iss >> token;
//
//			if (token == "vertex") {
//				iss >> header.numVertices;
//			}
//			else if (token == "face") {
//				iss >> header.numFaces;
//			}
//		}
//		else if (token == "property") {
//			PlyProperty property;
//			iss >> property.type >> property.name;
//
//			if (header.vertexProperties.size() < static_cast<size_t>(header.numVertices)) {
//				header.vertexProperties.push_back(property);
//			}
//			else {
//				header.faceProperties.push_back(property);
//			}
//			std::pair<size_t, size_t> start_end = { header.vertex_offset, 0 };
//
//			if (property.type == "char" || property.type == "int8")
//				header.vertex_offset += 1;
//			else if (property.type == "uchar" || property.type == "uint8")
//				header.vertex_offset += 1;
//			else if (property.type == "short" || property.type == "int16")
//				header.vertex_offset += 2;
//			else if (property.type == "ushort" || property.type == "uint16")
//				header.vertex_offset += 2;
//			else if (property.type == "int" || property.type == "int32")
//				header.vertex_offset += 4;
//			else if (property.type == "uint" || property.type == "uint32")
//				header.vertex_offset += 4;
//			else if (property.type == "float" || property.type == "float32")
//				header.vertex_offset += 4;
//			else if (property.type == "double" || property.type == "float64")
//				header.vertex_offset += 8;
//			else
//				std::cout << "Unsupported type " << property.type.c_str() << " for property" << property.name << std::endl;
//
//			start_end.second = header.vertex_offset;
//
//			std::string key;
//			if (property.name == "x" || property.name == "y" || property.name == "z")
//				header.UpdateVerticesOffset("position", start_end);
//			else if (property.name == "nx" || property.name == "ny" || property.name == "nz")
//				header.UpdateVerticesOffset("normal", start_end);
//			else if (property.name == "opacity")
//				header.UpdateVerticesOffset("opacity", start_end);
//			else if (property.name.find("rot") != std::string::npos)
//				header.UpdateVerticesOffset("rot", start_end);
//			else if (property.name.find("scale") != std::string::npos)
//				header.UpdateVerticesOffset("scale", start_end);
//			else if (property.name.find("f_rest") != std::string::npos || property.name.find("f_dc") != std::string::npos)
//				header.UpdateVerticesOffset("shs", start_end);
//		}
//		else if (token == "end_header") {
//			headerEnd = true;
//			break;
//		}
//	}
//	if (!headerEnd) {
//		throw std::runtime_error("Could not find end of header");
//	}
//}
//
//
//void SplatRenderer::SetupShader(const char* vertexPath, const char* fragmentPath, const char* presortPath, const char* sortPath)
//{
//	m_renderProgram = std::make_shared<Shader>(vertexPath, fragmentPath);
//#ifdef USE_GPU_SORT
//	assert(presortPath != nullptr);
//	assert(sortPath != nullptr);
//	m_presortProgram = std::make_shared<ComputeShader>(presortPath);
//	m_sortProgram = std::make_shared<ComputeShader>(sortPath);
//#endif // USE_GPU_SORT
//}
//
//void SplatRenderer::GenerateTex()
//{
//	Texture::Params m_texParams;
//	m_texParams.minFilter = FilterType::Nearest;
//	m_texParams.magFilter = FilterType::Nearest;
//	m_texParams.sWrap = WrapType::ClampToEdge;
//	m_texParams.tWrap = WrapType::ClampToEdge;
//	m_gaussian_texture = std::make_shared<Texture>(m_texwidth, m_texheight, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, m_texParams, m_texdata.data());
//}
//
//void SplatRenderer::SetupVertexBuffer()
//{
//
//	m_renderBufferVAO = std::make_shared<VertexArrayObject>();
//	// axis of gaussian rectangle
//	float position_scale = 1.0f;
//	std::vector<glm::vec2> rectangleVertices{
//		glm::vec2(-position_scale, -position_scale),
//		glm::vec2(position_scale, -position_scale),
//		glm::vec2(position_scale,  position_scale),
//		glm::vec2(-position_scale,  position_scale)
//	};
//	m_rectangleBuffer = std::make_shared<BufferObject>(GL_ARRAY_BUFFER, rectangleVertices, GL_STATIC_DRAW);
//	m_renderBufferVAO->SetAttribBuffer(0, m_rectangleBuffer);
//
//#ifndef USE_GPU_SORT
//	m_depthIndexBuffer = std::make_shared<BufferObject>(GL_ARRAY_BUFFER, m_depthIndex, GL_DYNAMIC_DRAW);
//	m_depthIndexBuffer->SetType(GL_INT);
//	m_depthIndexBuffer->SetOffset(1);
//	m_renderBufferVAO->SetAttribBuffer(1, m_depthIndexBuffer);
//#endif // !USE_GPU_SORT
//	m_renderBufferVAO->Bind();
//	glVertexAttribDivisor(m_renderProgram->GetAttribLocation("index"), 1);
//	m_renderBufferVAO->Unbind();
//}
//
//#ifdef USE_GPU_SORT
//void SplatRenderer::SetupComputeBuffer()
//{
//	// initialization
//	m_depthVec.resize(m_vertexCount);
//	m_indexVec.reserve(m_vertexCount);
//	for (uint32_t i = 0; i < (uint32_t)m_vertexCount; i++)
//	{
//		m_indexVec.emplace_back(i);
//	}
//
//	//  bool useMultiRadixSort = GLEW_KHR_shader_subgroup && !useRgcSortOverride;
//	m_atomicCounterBuffer = std::make_shared<BufferObject>(GL_ATOMIC_COUNTER_BUFFER, m_atomicCounterVec, GL_DYNAMIC_DRAW);
//	m_presortUniformBuffer = std::make_shared<BufferObject>(GL_UNIFORM_BUFFER, m_vertexCount, GL_DYNAMIC_DRAW);
//
//	m_sortUniformBuffer = std::make_shared<BufferObject>(GL_UNIFORM_BUFFER, m_sortUniform, GL_DYNAMIC_DRAW);
//	m_posBuffer = std::make_shared<BufferObject>(GL_SHADER_STORAGE_BUFFER, m_posVec, GL_DYNAMIC_DRAW);
//	m_keyBuffer = std::make_shared<BufferObject>(GL_SHADER_STORAGE_BUFFER, m_depthVec, GL_DYNAMIC_DRAW);
//	m_keyBuffer2 = std::make_shared<BufferObject>(GL_SHADER_STORAGE_BUFFER, m_depthVec, GL_DYNAMIC_DRAW);
//	m_valBuffer = std::make_shared<BufferObject>(GL_SHADER_STORAGE_BUFFER, m_indexVec, GL_DYNAMIC_DRAW);
//	m_valBuffer2 = std::make_shared<BufferObject>(GL_SHADER_STORAGE_BUFFER, m_indexVec, GL_DYNAMIC_DRAW);
//}
//
//
//void SplatRenderer::I_RunSortShader(glm::mat4& modelViewProjMatrix, const glm::vec2& projParams, const ImguiParameters& imguiParams)
//{
//	// runsort
//	{
//		//const uint32_t MAX_DEPTH = 0x80000000;
//		const uint32_t MAX_DEPTH = std::numeric_limits<uint32_t>::max();
//		m_presortProgram->use();
//		m_presortProgram->setMat4("modelViewProj", modelViewProjMatrix);
//		m_presortProgram->setVec2("nearFar", projParams);
//		m_presortProgram->setUint("keyMax", MAX_DEPTH);
//		m_atomicCounterVec = 0;
//		m_atomicCounterBuffer->Update(m_atomicCounterVec);
//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_posBuffer->GetObj());  // readonly
//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_keyBuffer->GetObj());  // writeonly
//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_valBuffer->GetObj());  // writeonly
//		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 3, m_atomicCounterBuffer->GetObj());
//
//		const int LOCAL_SIZE = 256;
//		glDispatchCompute(((GLuint)m_vertexCount + (LOCAL_SIZE - 1)) / LOCAL_SIZE, 1, 1); // Assuming LOCAL_SIZE threads per group
//		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
//		GL_ERROR_CHECK("SplatRenderer::Sort() pre-sort");
//	}
//	{
//		m_atomicCounterBuffer->Read(m_atomicCounterVec);
//		assert(m_atomicCounterVec <= (uint32_t)m_vertexCount);
//		GL_ERROR_CHECK("SplatRenderer::Render() get-count");
//	}
//	{
//		m_sortProgram->use();
//		m_sortUniform = sortUniform{
//			m_vertexCount,
//			static_cast<uint32_t>(imguiParams.sort_order)
//		};
//		m_sortUniformBuffer->Update(m_sortUniform);
//		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_sortUniformBuffer->GetObj());
//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_keyBuffer->GetObj());
//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_keyBuffer2->GetObj());
//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_valBuffer->GetObj());
//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_valBuffer2->GetObj());
//		glDispatchCompute(1, 1, 1);
//		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//
//		std::vector<uint32_t> sortedKeyVecOutput(m_vertexCount, 0);
//		m_keyBuffer->Read(sortedKeyVecOutput);
//		std::vector<uint32_t> sortedValueVecOutput(m_vertexCount, 0);
//		m_valBuffer->Read(sortedValueVecOutput);
//
//		GL_ERROR_CHECK("SplatRenderer::Sort() single radix sort");
//	}
//	{
//		m_renderProgram->use();
//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_valBuffer->GetObj());
//		GL_ERROR_CHECK("SplatRenderer::Sort() binding");
//	}
//}
//#else
//
//void PlyModel::I_RunSortUpdateDepth(glm::mat4& modelViewProjMatrix, bool useRadixSort, SORT_ORDER sortOrder)
//{
//	RunSortUpdateDepth<PlyVertex3>(m_vertices, modelViewProjMatrix, useRadixSort, sortOrder);
//}
//
//void SplatModel::I_RunSortUpdateDepth(glm::mat4& modelViewProjMatrix, bool useRadixSort, SORT_ORDER sortOrder)
//{
//	RunSortUpdateDepth<SplatVertex>(m_vertices, modelViewProjMatrix, useRadixSort, sortOrder);
//}
//
//
//#endif
//
//struct MinMax {
//	float min;
//	float max;
//};
//
//MinMax calculateMinMax(const std::vector<float>& data) {
//	float min, max;
//	min = max = data[0];
//
//	for (float item : data) {
//		if (item < min) min = item;
//		if (item > max) max = item;
//	}
//	return { min, max };
//}
//
//unsigned int Part1By2(unsigned int x) {
//	x &= 0x000003ff;
//	x = (x ^ (x << 16)) & 0xff0000ff;
//	x = (x ^ (x << 8)) & 0x0300f00f;
//	x = (x ^ (x << 4)) & 0x030c30c3;
//	x = (x ^ (x << 2)) & 0x09249249;   x = (x ^ (x << 2)) & 0x09249249;
//	return x;
//}
//
//
//unsigned int encodeMorton3(unsigned int x, unsigned int y, unsigned int z) {
//	return (Part1By2(z) << 2) + (Part1By2(y) << 1) + Part1By2(x);
//}
//
//template <typename T>
//void SplatRenderer::SortIndices(std::vector<T>& vertices)
//{
//	m_indices.resize(m_vertexCount);
//	for (size_t i = 0; i < m_indices.size(); i++) {
//		m_indices[i] = i;
//	}
//#ifndef USE_GPU_SORT
//	if (1) {
//		std::vector<float> x;
//		std::vector<float> y;
//		std::vector<float> z;
//		for (int i = 0; i < m_vertexCount; i++) {
//			x.push_back(-vertices[i].position[0]);
//			y.push_back(-vertices[i].position[1]);
//			z.push_back(vertices[i].position[2]);
//		}
//
//		MinMax bx = calculateMinMax(x);
//		MinMax by = calculateMinMax(y);
//		MinMax bz = calculateMinMax(z);
//
//		std::vector<unsigned int> morton(m_indices.size());
//		for (size_t i = 0; i < m_indices.size(); ++i) {
//			int idx = m_indices[i];
//			unsigned int ix = static_cast<unsigned int>(1024 * (x[idx] - bx.min) / (bx.max - bx.min));
//			unsigned int iy = static_cast<unsigned int>(1024 * (y[idx] - by.min) / (by.max - by.min));
//			unsigned int iz = static_cast<unsigned int>(1024 * (z[idx] - bz.min) / (bz.max - bz.min));
//			morton[i] = encodeMorton3(ix, iy, iz);
//		}
//
//		std::sort(m_indices.begin(), m_indices.end(), [&morton, &vertices](const int& a, const int& b) {
//			return morton[a] < morton[b];
//			});
//	}
//	else {
//		std::sort(m_indices.begin(), m_indices.end(), [&](const int& a, const int& b) {
//			auto& va = vertices[a];
//			auto& vb = vertices[b];
//			return va.opacity < vb.opacity;
//			});
//	}
//#endif // !USE_GPU_SORT
//}
//
//void PlyModel::LoadVertices(std::ifstream& file)
//{
//	m_vertices.resize(m_header.numVertices);
//	const float SH_C0 = 0.28209479177387814f;
//	for (auto i = 0; i < m_header.numVertices; i++) {
//		//static_assert(sizeof(PlyVertexStorage) == 62 * sizeof(float), "");
//		assert(file.is_open());
//		assert(!file.eof());
//		PlyVertexStorage vertexBuffer;
//		memset(&vertexBuffer, 0, sizeof(vertexBuffer));
//
//		for (auto property : m_header.verticePropertiesOrder) {
//			if (m_header.verticePropertiesOffset.find(property) != m_header.verticePropertiesOffset.end()) {
//				if (property == "position")
//					file.read(reinterpret_cast<char*>(&vertexBuffer.position), m_header.verticePropertiesOffset[property].second);
//				else if (property == "normal")
//					file.read(reinterpret_cast<char*>(&vertexBuffer.normal), m_header.verticePropertiesOffset[property].second);
//				else if (property == "shs")
//					file.read(reinterpret_cast<char*>(&vertexBuffer.shs), m_header.verticePropertiesOffset[property].second);
//				else if (property == "opacity")
//					file.read(reinterpret_cast<char*>(&vertexBuffer.opacity), m_header.verticePropertiesOffset[property].second);
//				else if (property == "scale")
//					file.read(reinterpret_cast<char*>(&vertexBuffer.scale), m_header.verticePropertiesOffset[property].second);
//				else if (property == "rot")
//					file.read(reinterpret_cast<char*>(&vertexBuffer.rotation), m_header.verticePropertiesOffset[property].second);
//			}
//		}
//
//		m_vertices[i].position = glm::vec4(vertexBuffer.position, 1.0f);
//		m_vertices[i].scale = glm::exp(vertexBuffer.scale);
//		m_vertices[i].opacity = 1.0f / (1.0f + std::exp(-vertexBuffer.opacity)); // sigmoid
//		m_vertices[i].rotation = glm::normalize(vertexBuffer.rotation);
//		m_vertices[i].shs[0] = vertexBuffer.shs[0] * SH_C0;
//		m_vertices[i].shs[1] = vertexBuffer.shs[1] * SH_C0;
//		m_vertices[i].shs[2] = vertexBuffer.shs[2] * SH_C0;
//
//		auto SH_N = m_header.verticePropertiesOffset["shs"].second / sizeof(float) / 3;
//		for (auto j = 1; j < SH_N; j++) {
//			m_vertices[i].shs[j * 3 + 0] = vertexBuffer.shs[(j - 1) + 3];
//			m_vertices[i].shs[j * 3 + 1] = vertexBuffer.shs[(j - 1) + SH_N + 2];
//			m_vertices[i].shs[j * 3 + 2] = vertexBuffer.shs[(j - 1) + SH_N * 2 + 1];
//		}
//
//		//assert(vertexBuffer.normal.x == 0.0f);
//		//assert(vertexBuffer.normal.y == 0.0f);
//		//assert(vertexBuffer.normal.z == 0.0f);
//
//#ifdef USE_GPU_SORT
//		m_posVec.emplace_back(glm::vec4(m_vertices[i].position.x, m_vertices[i].position.y, m_vertices[i].position.z, 1.0f));
//#endif // USE_GPU_SORT
//	}
//	file.close();
//}
//
//void PlyModel::GenerateTexData()
//{
//	for (size_t i = 0; i < m_vertexCount; i++) {
//		size_t idx = m_indices[i];
//		glm::vec3 pos = m_vertices[idx].position;
//		glm::vec3* sh = reinterpret_cast<glm::vec3*>(&m_vertices[idx].shs);
//		glm::vec3 result = SH_C0 * sh[0];
//		result += 0.5f;
//		result = (glm::max)(result, glm::vec3(0.0f));
//		result = (glm::min)(result, glm::vec3(1.0f));
//		glm::vec4 shs = glm::vec4(result, m_vertices[idx].opacity) * 255.0f;
//		uint8_t shs_uint8[4]{ static_cast<uint8_t>(shs.x), static_cast<uint8_t>(shs.y), static_cast<uint8_t>(shs.z), static_cast<uint8_t>(shs.w) };
//		std::vector<float> sigmas;
//		GetSigmaFloat32(m_vertices[idx].rotation, m_vertices[idx].scale, sigmas);
//		// 0: posx, 1: posy, 2: posz, 3: 1, 4: cov1, 5: cov2, 6: cov3, 7: cov4, 8: cov5, 9: cov6, 10: RGBA(lp), 11: opacity(hp), 12-60: shs
//		std::copy_n(reinterpret_cast<float*>(&m_vertices[idx].position), 4, reinterpret_cast<float*>(&m_texdata[m_vertexLength * i]));
//		std::copy_n(reinterpret_cast<float*>(sigmas.data()), 6, reinterpret_cast<float*>(&m_texdata[m_vertexLength * i + 4]));
//		std::copy_n(reinterpret_cast<uint8_t*>(&shs_uint8), 4, reinterpret_cast<uint8_t*>(&m_texdata[m_vertexLength * i + 10]));
//		std::copy_n(reinterpret_cast<float*>(&m_vertices[idx].opacity), 1, reinterpret_cast<float*>(&m_texdata[m_vertexLength * i + 11]));
//		std::copy_n(reinterpret_cast<float*>(&m_vertices[idx].shs), (std::max)(static_cast<int>(m_header.verticePropertiesOffset["shs"].second / sizeof(float)), 3),
//			reinterpret_cast<float*>(&m_texdata[m_vertexLength * i + 12]));
//	}
//	GenerateTex();
//}
//
//PlyModel::PlyModel(std::ifstream& file, PlyHeader& header, const char* vertexPath, const char* fragmentPath)
//{
//	{ // io
//		m_header = header;
//		m_type = MODEL_TYPE::PLY;
//		m_vertexCount = m_header.numVertices;
//		m_vertexLength = 64;
//		m_texwidth = 1024 * 2;
//		m_texheight = std::ceil((2.0f * m_vertexCount) / m_texwidth) * 8;
//		m_texdata.resize(m_texwidth * m_texheight * 4);
//		LoadVertices(file);
//		SortIndices(m_vertices); // TODO: a heuristic algorithm currently
//		GenerateTexData();
//	}
//	GenerateAABB();
//#ifdef USE_GPU_SORT
//	m_sortUniform = sortUniform{ m_vertexCount, static_cast<uint32_t>(SORT_ORDER::DESCENDING) };
//	SetupShader(vertexPath, fragmentPath, "./shader/presort_comp.glsl", "./shader/single_radix_sort.glsl");
//	SetupComputeBuffer();
//#else
//	m_depthIndex.resize(m_vertexCount);
//	SetupShader(vertexPath, fragmentPath);
//#endif
//	SetupVertexBuffer();
//}
//
//void PlyModel::Render(const vertexUniform& uniform, const ImguiParameters& imguiParams, bool isDirty)
//{
//	//m_renderProgram->use();
//	//m_renderBufferVAO->Bind();
//	//m_gaussian_texture->Bind(uniform.u_texture);
//	//m_renderProgram->setInt("u_texture", uniform.u_texture);
//	//m_renderProgram->setMat4("model", uniform.model);
//	//m_renderProgram->setMat4("view", uniform.view);
//	//m_renderProgram->setMat4("projection", uniform.projection);
//	//m_renderProgram->setVec2("focal", uniform.focal);
//	//m_renderProgram->setVec2("viewport", uniform.viewport);
//	//m_renderProgram->setVec3("cam_pos", uniform.cam_pos);
//	//m_renderProgram->setVec2("tan_fov", uniform.tan_fov);
//	//m_renderProgram->setVec2("projParams", uniform.projParams);
//	//m_renderProgram->setInt("sh_deg", uniform.sh_deg);
//	//m_renderProgram->setInt("showGaussian", uniform.showGaussian ? 1 : 0);
//	//m_renderProgram->setInt("showHotspots", uniform.showHotspots ? 1 : 0);
//
//	const bool use_gpu_sort = false;
//	if (isDirty) {
//		glm::mat4 modelViewProjMatrix = uniform.projection * uniform.view * uniform.model;
//
//#ifdef USE_GPU_SORT
//		I_RunSortShader(modelViewProjMatrix, uniform.projParams, imguiParams);
//#else
//		I_RunSortUpdateDepth(modelViewProjMatrix, imguiParams.useRadixSort, imguiParams.sort_order);
//#endif
//	}
//	if (uniform.showGaussian) {
//		glDrawArraysInstanced(GL_POINTS, 0, 1, imguiParams.vertexCount);
//	}
//	else {
//		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, imguiParams.vertexCount);
//	}
//	m_renderBufferVAO->Unbind();
//	//GL_ERROR_CHECK("PlyModel::Render");
//}
//
//void PlyModel::GetSigmaFloat32(glm::vec4& _rotation, glm::vec3& _scale, std::vector<float>& sigmaFloat32)
//{
//	std::vector<double> rot{
//		static_cast<double>(_rotation[0]),
//		static_cast<double>(_rotation[1]),
//		static_cast<double>(_rotation[2]),
//		static_cast<double>(_rotation[3]),
//	};
//
//	std::vector<double> scale{
//		static_cast<double>(_scale.x),
//		static_cast<double>(_scale.y),
//		static_cast<double>(_scale.z),
//	};
//
//	std::vector<double> sigma(6);
//	GetSigma(scale, rot, sigma);
//
//	for (size_t i = 0; i < 6; i++) {
//		sigmaFloat32.emplace_back(sigma[i]);
//	}
//}
//
//SplatModel::SplatModel(const std::string& filename, const char* vertexPath, const char* fragmentPath)
//{
//	{ // io
//		std::ifstream file(filename, std::ios::binary);
//		if (!file.is_open()) {
//			std::cerr << "Unable to open the file:" << filename << std::endl;
//		}
//		else {
//			file.seekg(0, std::ios::end);
//			std::streamsize size = file.tellg();
//			size_t rowLength = 3 * 4 + 3 * 4 + 4 + 4;
//			m_vertexCount = size / rowLength;
//		}
//		m_type = MODEL_TYPE::SPLAT;
//		m_vertexLength = 8;
//		m_texwidth = 1024 * 2;
//		m_texheight = std::ceil((2.0f * m_vertexCount) / m_texwidth);
//		m_texdata.resize(m_texwidth * m_texheight * 4);
//		LoadVertices(file);
//	}
//	GenerateAABB();
//#ifdef USE_GPU_SORT
//	m_sortUniform = sortUniform{ m_vertexCount, static_cast<uint32_t>(SORT_ORDER::ASCENDING) };
//	SetupShader(vertexPath, fragmentPath, "./shader/presort_comp.glsl", "./shader/single_radix_sort.glsl");
//	SetupComputeBuffer();
//#else
//	m_depthIndex.resize(m_vertexCount);
//	SetupShader(vertexPath, fragmentPath);
//#endif
//	SetupVertexBuffer();
//}
//
//void SplatModel::LoadVertices(std::ifstream& file)
//{
//	file.seekg(0, std::ios::beg);
//	m_vertices.resize(m_vertexCount);
//	for (size_t i = 0; i < m_vertexCount; i++) {
//		assert(file.is_open(), "");
//		file.read(reinterpret_cast<char*>(&m_vertices[i]), sizeof(SplatVertex));
//		std::vector<uint32_t> sigmasHalf2x16;
//		SplatModel::GetSigmaHalf2x16(m_vertices[i], sigmasHalf2x16);
//		// 0: posx, 1: posy, 2: posz, 3: 0, 4: cov12, 5: cov34, 6: cov56, 7: RGBA(lp)
//		std::copy_n(reinterpret_cast<float*>(&m_vertices[i].position), 3, reinterpret_cast<float*>(&m_texdata[m_vertexLength * i]));
//		std::copy_n(reinterpret_cast<uint32_t*>(sigmasHalf2x16.data()), 3, reinterpret_cast<uint32_t*>(&m_texdata[m_vertexLength * i + 4]));
//		std::copy_n(reinterpret_cast<uint8_t*>(&m_vertices[i].shs), 4, reinterpret_cast<uint8_t*>(&m_texdata[m_vertexLength * i + 7]));
//
//#ifdef USE_GPU_SORT
//		m_posVec.emplace_back(glm::vec4(m_vertices[i].position.x, m_vertices[i].position.y, m_vertices[i].position.z, 1.0f));
//#endif // USE_GPU_SORT
//	}
//	GenerateTex();
//}
//
//void SplatModel::GetSigmaHalf2x16(SplatVertex& vertexBuffer, std::vector<uint32_t>& sigmasHalf2x16)
//{
//	std::vector<double> rot{
//		(static_cast<double>(vertexBuffer.rotation[0]) - 128.0) / 128.0,
//		(static_cast<double>(vertexBuffer.rotation[1]) - 128.0) / 128.0,
//		(static_cast<double>(vertexBuffer.rotation[2]) - 128.0) / 128.0,
//		(static_cast<double>(vertexBuffer.rotation[3]) - 128.0) / 128.0,
//	};
//
//	std::vector<double> scale{
//		static_cast<double>(vertexBuffer.scale.x),
//		static_cast<double>(vertexBuffer.scale.y),
//		static_cast<double>(vertexBuffer.scale.z),
//	};
//
//	std::vector<double> sigma(6);
//	GetSigma(scale, rot, sigma);
//
//	for (size_t i = 0; i < 3; i++) {
//		sigmasHalf2x16.emplace_back(packHalf2x16(sigma[i * 2], sigma[i * 2 + 1]));
//	}
//}
//
//
//void SplatModel::Render(const vertexUniform& uniform, const ImguiParameters& imguiParams, bool isDirty)
//{
//	m_renderProgram->Use();
//	m_renderBufferVAO->Bind();
//	m_gaussian_texture->BindTexture(uniform.u_texture);
//	m_renderProgram->SetInt("u_texture", uniform.u_texture);
//	m_renderProgram->SetMat4("model", uniform.model);
//	m_renderProgram->SetMat4("view", uniform.view);
//	m_renderProgram->SetMat4("projection", uniform.projection);
//	m_renderProgram->SetVec2("focal", uniform.focal);
//	m_renderProgram->SetVec2("viewport", uniform.viewport);
//	if (isDirty) {
//		glm::mat4 modelViewProjMatrix = uniform.projection * uniform.view * uniform.model;
//#ifdef USE_GPU_SORT
//		I_RunSortShader(modelViewProjMatrix, uniform.projParams, imguiParams);
//#else
//		I_RunSortUpdateDepth(modelViewProjMatrix, imguiParams.useRadixSort, imguiParams.sort_order);
//#endif
//	}
//	if (uniform.showGaussian) {
//		glDrawArraysInstanced(GL_POINTS, 0, 1, m_vertexCount);
//	}
//	else {
//		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, m_vertexCount);
//	}
//	m_renderBufferVAO->Unbind();
//}
//
//PackedPlyModel::PackedPlyModel(std::ifstream& file, PackPlyHeader& header, const char* vertexPath, const char* fragmentPath)
//{
//	{ // io
//		m_header = header;
//		assert(m_header.element2offest["vertex"] == sizeof(VertexData));
//		assert(m_header.element2offest["chunk"] == sizeof(ChunkData));
//		m_type = MODEL_TYPE::PACKED_PLY;
//		m_vertexCount = m_header.numVertices;
//		m_vertexLength = 64;
//		m_texwidth = 1024 * 2;
//		m_texheight = std::ceil((2.0f * m_vertexCount) / m_texwidth) * 8;
//		m_texdata.resize(m_texwidth * m_texheight * 4);
//		LoadVertices(file);
//		SortIndices(m_vertices); // TODO: a heuristic algorithm currently
//		GenerateTexData();
//	}
//	GenerateAABB();
//#ifdef USE_GPU_SORT
//	m_sortUniform = sortUniform{ m_vertexCount, static_cast<uint32_t>(SORT_ORDER::DESCENDING) };
//	SetupShader(vertexPath, fragmentPath, "./shader/presort_comp.glsl", "./shader/single_radix_sort.glsl");
//	SetupComputeBuffer();
//#else
//	m_depthIndex.resize(m_vertexCount);
//	SetupShader(vertexPath, fragmentPath);
//#endif
//	SetupVertexBuffer();
//}
//
//float unpackUnorm(uint32_t value, size_t bits) {
//	uint32_t t = (1 << bits) - 1;
//	return (value & t) / static_cast<float>(t);
//}
//
//void unpack111011(glm::vec3& result, uint32_t value) {
//	result.x = unpackUnorm(value >> 21, 11);
//	result.y = unpackUnorm(value >> 11, 10);
//	result.z = unpackUnorm(value, 11);
//}
//
//void unpack8888(glm::vec4& result, uint32_t value) {
//	result.x = unpackUnorm(value >> 24, 8);
//	result.y = unpackUnorm(value >> 16, 8);
//	result.z = unpackUnorm(value >> 8, 8);
//	result.w = unpackUnorm(value, 8);
//}
//
//void unpackRot(glm::vec4& result, uint32_t value) {
//	float norm = 1.0f / (sqrt(2.0) * 0.5);
//	float a = (unpackUnorm(value >> 20, 10) - 0.5) * norm;
//	float b = (unpackUnorm(value >> 10, 10) - 0.5) * norm;
//	float c = (unpackUnorm(value, 10) - 0.5) * norm;
//	float m = sqrt(1.0f - (a * a + b * b + c * c));
//
//	switch (value >> 30) {
//	case 0: result = glm::vec4(m, a, b, c); break;
//	case 1: result = glm::vec4(a, m, b, c); break;
//	case 2: result = glm::vec4(a, b, m, c); break;
//	case 3: result = glm::vec4(a, b, c, m); break;
//	}
//}
//
//float lerp(float a, float b, float t) {
//	return a * (1 - t) + b * t;
//}
//
//void PackedPlyModel::LoadVertices(std::ifstream& file)
//{
//	VertexData vertex_data;
//	m_vertices.resize(m_header.numVertices);
//	std::vector<char> vertex_buffer(m_header.element2offest["vertex"], 0);
//	std::vector<char> chunk_buffer(m_header.element2offest["chunk"], 0);
//
//	std::vector<ChunkData> chunk_datas(m_header.numChunks);
//	for (size_t i = 0; i < m_header.numChunks; i++) {
//		memset(&chunk_datas[i], 0, sizeof(ChunkData));
//		file.read(reinterpret_cast<char*>(&chunk_datas[i]), sizeof(ChunkData));
//	}
//
//	glm::vec3 p; // position
//	glm::vec4 r; // quaternion
//	glm::vec3 s; // scale
//	glm::vec4 c; // color
//
//	for (size_t i = 0; i < m_header.numVertices; i++) {
//		size_t ci = floor(i / 256);
//
//		assert(ci < m_header.numChunks);
//		memset(&vertex_data, 0, sizeof(VertexData));
//		file.read(reinterpret_cast<char*>(&vertex_data), sizeof(VertexData));
//		unpack111011(p, vertex_data.position);
//		unpackRot(r, vertex_data.rotation);
//		unpack111011(s, vertex_data.scale);
//		unpack8888(c, vertex_data.color);
//
//		m_vertices[i].position.x = -lerp(chunk_datas[ci].min_x, chunk_datas[ci].max_x, p.x);
//		m_vertices[i].position.y = -lerp(chunk_datas[ci].min_y, chunk_datas[ci].max_y, p.y);
//		m_vertices[i].position.z = lerp(chunk_datas[ci].min_z, chunk_datas[ci].max_z, p.z);
//		m_vertices[i].position.w = 1.0f;
//
//		m_vertices[i].rotation.x = r.w;
//		m_vertices[i].rotation.y = r.z;
//		m_vertices[i].rotation.z = -r.y;
//		m_vertices[i].rotation.w = -r.x;
//		m_vertices[i].rotation = glm::normalize(m_vertices[i].rotation);
//
//		m_vertices[i].scale.x = glm::exp(lerp(chunk_datas[ci].min_scale_x, chunk_datas[ci].max_scale_x, s.x));
//		m_vertices[i].scale.y = glm::exp(lerp(chunk_datas[ci].min_scale_y, chunk_datas[ci].max_scale_y, s.y));
//		m_vertices[i].scale.z = glm::exp(lerp(chunk_datas[ci].min_scale_z, chunk_datas[ci].max_scale_z, s.z));
//
//		m_vertices[i].shs[0] = (c.x - 0.5);
//		m_vertices[i].shs[1] = (c.y - 0.5);
//		m_vertices[i].shs[2] = (c.z - 0.5);
//		m_vertices[i].opacity = c.w;
//	}
//}