#ifndef TEXTURE_H
#define TEXTURE_H
#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>

class Texture
{
public:
	Texture(const std::string path);
	~Texture();

	void Bind(unsigned int slot = 0)const;
	void Unbind()const;

	inline int GetWidth()const { return m_Width; }
	inline int GetHeight()const { return m_Height; }

private:
	unsigned int m_RendererID;
	const std::string m_Filepath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, nrChannels;// byte per pixel
};


Texture::Texture(const std::string path)
	: m_RendererID(0), m_Filepath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0)
{
	const std::string sub_str = path.substr(path.find_last_of('.') + 1);
	nrChannels = (sub_str == "png") ? 4 : 3;

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	m_LocalBuffer = stbi_load(path.data(), &m_Width, &m_Height, &nrChannels, 0);
	if (m_LocalBuffer) {
		if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		else if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer);
		else
			std::cout << "Wrong texture format." << std::endl;

		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(m_LocalBuffer);
	}
	else {
		std::cout << "Failed to load texture." << std::endl;
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

#endif