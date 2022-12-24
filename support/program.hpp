#ifndef PROGRAM_HPP_39793FD2_7845_47A7_9E21_6DDAD42C9A09
#define PROGRAM_HPP_39793FD2_7845_47A7_9E21_6DDAD42C9A09

#include <glad.h>

#include <string>
#include <vector>

#include <cstdint>
#include <cstdlib>

enum class LightModel {BlinnPhong, PBR};

class ShaderProgram final
{
	public:
		struct ShaderSource
		{
			GLenum type;
			std::string sourcePath;
		};

	public:
		explicit ShaderProgram(std::vector<ShaderSource> = {});

		~ShaderProgram();

		ShaderProgram( ShaderProgram const& ) = delete;
		ShaderProgram& operator= (ShaderProgram const&) = delete;

		ShaderProgram( ShaderProgram&& ) noexcept;
		ShaderProgram& operator= (ShaderProgram&&) noexcept;

	public:
		GLuint programId() const noexcept;

		void reload();
	private:
		GLuint mProgram;
		std::vector<ShaderSource> mSources;
};

class RenderSettings {
private:
	static const int MAX_CODES = 2;
	std::vector<ShaderProgram*> programs;

public:
	RenderSettings(LightModel model);
	void setProgram(int code, ShaderProgram* program);
	void reloadPrograms();
	ShaderProgram* getProgram(int code) const;
	LightModel lightModel;

	static const int STANDARD = 0;
	static const int BUMP_MAP = 1;
};

#endif // PROGRAM_HPP_39793FD2_7845_47A7_9E21_6DDAD42C9A09
