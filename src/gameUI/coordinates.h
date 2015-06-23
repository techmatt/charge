
class CoordinateFrame
{
public:
	CoordinateFrame()
	{
		start = vec2f(0.0f, 0.0f);
		end = vec2f(0.0f, 0.0f);
		internalDimensions = vec2f(1.0f, 1.0f);
		frameScale = vec2f(1.0f, 1.0f);
	}

	CoordinateFrame(const vec2f &_start, const vec2f &_end)
	{
		start = _start;
		end = _end;
		internalDimensions = end - start;
		frameScale = vec2f(1.0f, 1.0f);
	}
	CoordinateFrame(const vec2f &_start, const vec2f &_end, const vec2f &_internalDimensions)
	{
		start = _start;
		end = _end;
		internalDimensions = _internalDimensions;
		frameScale = vec2f((end.x - start.x) / internalDimensions.x, (end.x - start.x) / internalDimensions.x);
	}
	CoordinateFrame(const rect2f &_shape)
	{
        start = _shape.min();
        end = _shape.max();
		internalDimensions = end - start;
		frameScale = vec2f(1.0f, 1.0f);
	}
	CoordinateFrame(const vec2f &_start)
	{
		start = _start;
		end = start + vec2f(1.0f,1.0f);
		internalDimensions = vec2f(1.0f, 1.0f);
		frameScale = vec2f(1.0f, 1.0f);
	}
	CoordinateFrame(const rect2f &_shape, const vec2f &_internalDimensions)
	{
        start = _shape.min();
        end = _shape.max();
		internalDimensions = _internalDimensions;
		frameScale = vec2f((end.x - start.x) / internalDimensions.x, (end.x - start.x) / internalDimensions.x);
	}
	CoordinateFrame(float xmin, float xmax, float xrange, float ymin, float ymax, float yrange)
	{
		start = vec2f(xmin,ymin);
		end = vec2f(xmax,ymax);
		internalDimensions = vec2f(xrange,yrange);
		frameScale = vec2f((end.x - start.x) / internalDimensions.x, (end.x - start.x) / internalDimensions.x);
	}


	// finds the shape of the canonical coordinate frame in that frame.
	//CoordinateFrame intervertCoordinateFrame(CoordinateFrame &frame);

	// given a coordinate frame defined in the coordinates of some other frame, convert between them
	rect2f toContainer(const rect2f &rect) const;
	vec2f toContainer(const vec2f &vec) const;

	rect2f fromContainer(const rect2f &rect) const ;
	vec2f fromContainer(const vec2f &rect) const ;

private:
	// start and end are defined in terms of the coordinates of the enclosing frame
	// in general, internalDimensions = end-start, but if you want to rescale things, it might not be,
	// for example, you can scale down by fitting an internally 100x100 circuit into a canonically 10x10 box
	vec2f start;
	vec2f end;
	vec2f internalDimensions;

	vec2f frameScale;	//=(end-start)/internalDimensions, always
};