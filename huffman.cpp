#include "huffman.h"
#include <stack>

HuffmanNode* huffman::buildHuffmanTree(const std::unordered_map<uint8_t, int>& frequencyTable)
{
	/*
	* Khi tạo cây Huffman, ta cần tạo một priority queue min heap từ bảng tần suất.
	* Việc nhân với số âm là để tạo min heap
	* Tạo priority queue min heap từ bảng tần suất
	*/
	std::priority_queue<std::pair<int, HuffmanNode*>> pq;
	for (const auto& pair : frequencyTable)
	{
		pq.push({ (-100) * pair.second, new HuffmanNode(pair.first, true, pair.second) }); // Đảo dấu để tạo min heap
	}

	// Xây dựng cây Huffman
	while (pq.size() > 1)
	{
		int leftFreq = pq.top().first;
		HuffmanNode* left = pq.top().second;
		pq.pop();

		int rightFreq = pq.top().first;
		HuffmanNode* right = pq.top().second;
		pq.pop();

		HuffmanNode* parent = new HuffmanNode(0, false, left->frequency + right->frequency);

	parent->left = left;
	parent->right = right;

	/*
	* Ở dòng tiếp theo ta trừ thêm 1 xem như chiều cao của cây cũng ảnh hưởng đến việc chọn nút
	* Theo đó nút càng thấp sẽ có độ ưu tiên càng cao với các nút có cùng giá trị tần suất
	* Giá trị -100 lúc đầu sẽ đảm bảo cho việc cây có độ cao không quá 100
	* Nếu vượt quá chiều cao này cây có thể sẽ không chính xác nhưng 100 là độ cao đủ lớn khó để đạt được
	* Cách này cũng giúp đầu ra ổn định với dữ liệu đầu vào có tần suất tương tự nhau
	*/

	pq.push({ leftFreq + rightFreq - 1, parent });
		}

	return pq.top().second;
}

void huffman::generateCodes(
	HuffmanNode* root,   // Nút hiện tại
	const std::vector<uint8_t>& code, // Mã hiện tại
	std::unordered_map<uint8_t, std::vector<uint8_t>>& codes)  // Bảng mã
{
	if (root->isLeaf)
	{
		codes[root->byte] = code;
		return;
	}

	std::vector<uint8_t> leftCode = code;
	leftCode.push_back(huffman::TOLEFT);
	generateCodes(root->left, leftCode, codes);

	std::vector<uint8_t> rightCode = code;
	rightCode.push_back(huffman::TORIGHT);
	generateCodes(root->right, rightCode, codes);
}

std::vector<uint8_t> huffman::encode(
	const std::vector<uint8_t>& data,
	std::vector<uint8_t>& trans)
{
	if (data.empty())
	{
		return data;
	}

	// Tạo Bảng tần suất
	std::unordered_map<uint8_t, int> frequencyTable = buildFrequencyTable(data);

	// Tạo cây Huffman
	HuffmanTree tree;
	tree.root = buildHuffmanTree(frequencyTable);

	// Chuyển đổi từ cây thành vector
	trans = writeTree(tree.root);

	// Tạo bảng mã Huffman
	generateCodes(tree.root, std::vector<uint8_t>(), tree.codes);

	// Tạo bitstream
	BitStream bitStream(data.size());
	for (uint8_t byte : data)
	{
		bitStream.pushBit(byte, tree.codes.at(byte));
	}

	return bitStream.getData();
}


std::vector<uint8_t> huffman::decode(
	const std::vector<uint8_t>& encodedData,
	HuffmanNode* root,
	const uint32_t origanalSize)
{
	BitStream bitStream(origanalSize);
	bitStream.setData(encodedData);

	std::vector<uint8_t> decodedData;
	while (decodedData.size() < origanalSize)
	{
		HuffmanNode* node = root;

		while (!node->isLeaf)
		{
			if (bitStream.getBits() == huffman::TOLEFT)
			{
				node = node->left;
			}
			else 
			{
				node = node->right;
			}
		}

		decodedData.push_back(node->byte);
	}

	return decodedData;
}

std::unordered_map<uint8_t, int> huffman::buildFrequencyTable(const std::vector<uint8_t>& data)
{
	std::unordered_map<uint8_t, int> frequencyTable;

	for (uint8_t byte : data)
	{
		frequencyTable[byte]++;
	}

	return frequencyTable;
}

std::vector<uint8_t> huffman::writeTree(HuffmanNode* root)
{
	std::vector<uint8_t> data;
	if (root == nullptr)
	{
		return data;
	}

	std::stack<HuffmanNode*> s;
	HuffmanNode* p = root;
	int indexZero = 0;

	while (p != nullptr || !s.empty())
	{
		while (p != nullptr)
		{
			data.push_back(p->byte);
			s.push(p);
			p = p->left;
		}

		if (!s.empty())
		{
			p = s.top();
			s.pop();

			if (p->byte == 0x0 && p->isLeaf)
			{
				indexZero = data.size();
			}

			p = p->right;
		}
	}

	// Tối đa 256 kí tự vậy indexZero không vượt quá 2n-1 = 511
	// Có thể lưu indexZerobằng 2 bytes
	// 2 bytes cuối cùng lưu indexZero

	if (indexZero > 511)
	{
		indexZero = 0xffff;  // Nếu trong data không tồn tại 0x0
	}

	data.push_back(indexZero & 0xff);
	data.push_back((indexZero >> 8) & 0xff);

	return data;
}

HuffmanNode* huffman::readTree(const std::vector<uint8_t>& treeData)
{
	// Lấy 2 bytes cuối cùng để lấy indexZero
	int indexZero = treeData[treeData.size() - 2] | (treeData[treeData.size() - 1] << 8);

	HuffmanNode* root = new HuffmanNode(0, false, 0);
	std::stack<HuffmanNode*> s;
	s.push(root);

	for (size_t i = 1; i < treeData.size() - 2; i++)  // Bỏ qua 2 byte cuối cùng
	{
		HuffmanNode* node = new HuffmanNode(treeData[i], treeData[i] != 0 || i == indexZero - 1, 0);

		if (s.top()->left == nullptr)
		{
			s.top()->left = node;
		}
		else
		{
			s.top()->right = node;
			s.pop();
		}

		if (!node->isLeaf)
		{
			s.push(node);
		}
	}

	return root;
}

HuffmanTree::~HuffmanTree()
{
	if (root == nullptr)
	{
		return;
	}

	std::queue<HuffmanNode*> q;
	q.push(root);
	while (!q.empty())
	{
		HuffmanNode* node = q.front();
		q.pop();

		if (node->left != nullptr)
		{
			q.push(node->left);
		}

		if (node->right != nullptr)
		{
			q.push(node->right);
		}

		delete node;
	}
}