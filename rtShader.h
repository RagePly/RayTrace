#pragma once

namespace RayTrace {
	unsigned int compProg(const char**, const unsigned int);
	bool _assertComp(const unsigned int, const char*);
	bool _assertLink(const unsigned int);

	class Shader {
	public:
		Shader();
		Shader(const char*, const char*);
		~Shader();

		void compileFromPath(const char*, const char*);

		void use();
		bool success() const;

		operator const unsigned int() const; //LMAO this actually exists. Atm no problem but keep an eye out. Because essentially its just a wrapper...

	private:
		unsigned int programId;
		bool compiled;
	};
}