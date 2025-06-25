#pragma once

#include <string_view>

class Renderer;
struct SDL_Texture;
struct SDL_Surface;

class Texture {
public:
	Texture() = default;
	Texture(Renderer& renderer, std::string_view path);
	~Texture();

	Texture(const Texture& other) = delete;
	Texture& operator=(const Texture& other) = delete;
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;

	SDL_Texture* get() const;
	int getWidth() const;
	int getHeight() const;

	operator bool() const;
	bool operator!() const;

	Texture& loadFromSurface(Renderer& renderer, SDL_Surface* surface);
	Texture& loadImage(Renderer& renderer, std::string_view path);

private:
	SDL_Texture* m_texture{ nullptr };
	int m_width{};
	int m_height{};
};