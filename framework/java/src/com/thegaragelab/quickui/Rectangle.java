/*---------------------------------------------------------------------------*
* $Id$
*----------------------------------------------------------------------------*
* May 23, 2013 - shane
* 
* Initial version
*---------------------------------------------------------------------------*/
package com.thegaragelab.quickui;

/** Extends Point to add width and height dimensions.
 */
public class Rectangle extends Point implements IRectangle {
  //--- Constants
  public static final Rectangle EMPTY = new Rectangle(Point.ORIGIN, Dimension.EMPTY);
  
  //--- Instance variables
  public int width;  //! Width of the rectangle
  public int height; //! Height of the rectangle

  //-------------------------------------------------------------------------
  // Construction and initialisation
  //-------------------------------------------------------------------------
  
  /** Constructor with top left corner and dimensions.
   * 
   * @param nx the X co-ordinate of the top left corner.
   * @param ny the Y co-ordinate of the top left corner.
   * @param w the width of the rectangle
   * @param h the height of the rectangle
   */
  public Rectangle(int nx, int ny, int w, int h) {
    super(nx, ny);
    width = w;
    height = h;
    }

  /** Constructor with a Point and dimension information
   * 
   * @param point the IPoint representing the top left left corner.
   * @param w the width of the rectangle.
   * @param h the height of the rectangle.
   */
  public Rectangle(IPoint point, int w, int h) {
    super(point);
    width = w;
    height = h;
    }
  
  /** Constructor with a Point and a Dimension
   * 
   * @param point the Point representing the top left left corner.
   * @param dimension the Dimension of the rectangle.
   */
  public Rectangle(IPoint point, IDimension dimension) {
    super(point);
    width = dimension.getWidth();
    height = dimension.getHeight();
    }
  
  /** Constructor with location information and a Dimension
   * 
   * @param nx the X co-ordinate of the top left corner.
   * @param ny the Y co-ordinate of the top left corner.
   * @param dimension the Dimension of the rectangle.
   */
  public Rectangle(int nx, int ny, IDimension dimension) {
    super(nx, ny);
    width = dimension.getWidth();
    height = dimension.getHeight();
    }
  
  /** Constructor from another Rectangle
   * 
   * @param rect the Rectangle to copy.
   */
  public Rectangle(IRectangle rect) {
    super(rect);
    width = rect.getWidth();
    height = rect.getHeight();
    }

  //-------------------------------------------------------------------------
  // Getters and setters
  //-------------------------------------------------------------------------
  
  /** Get the width of the rectangle.
   * 
   * @return the width of the rectangle.
   */
  public int getWidth() {
    return width;
    }
  
  /** Set the width of the rectangle.
   * 
   * @param w the new width of the rectangle.
   */
  public void setWidth(int w) {
    width = w;
    }
  
  /** Get the height of the rectangle.
   * 
   * @return the height of the rectangle.
   */
  public int getHeight() {
    return height;
    }
  
  /** Set the height of the rectangle.
   * 
   * @param h the new height of the rectangle.
   */
  public void setHeight(int h) {
    height = h;
    }
  
  /** Get the point portion of this rectangle as a new instance.
   * 
   * @return a Point instance representing the location of the rectangle.
   */
  public Point getPoint() {
    return new Point(this);
    }
  
  /** Get the dimension of this rectangle as a new instance.
   * 
   * @return a Dimension instance representing the size of the rectangle.
   */
  public Dimension getDimension() {
    return new Dimension(this);
    }
  
  //-------------------------------------------------------------------------
  // Implementation of IPoint
  //-------------------------------------------------------------------------
  
  /** Translate the point so the given point is the origin
   * 
   * @point origin the new origin for the co-ordinates
   * 
   * @return IPoint the translated instance.
   */
  public IPoint translate(IPoint origin) {
    return new Rectangle(x + origin.getX(), y + origin.getY(), width, height);
    }
  
  //-------------------------------------------------------------------------
  // Implementation of IRectangle
  //-------------------------------------------------------------------------
  
  /** Determine if this rectangle contains the given point
   * 
   * @param point the point to test.
   * 
   * @return true if the rectangle contains the given point, false otherwise.
   */
  public boolean contains(IPoint point) {
    if((point.getX()<x)||((point.getX()-x)>=width))
      return false;
    if((point.getY()<y)||((point.getY()-y)>=height))
      return false;
    return true;
    }
  
  //-------------------------------------------------------------------------
  // Static helpers
  //-------------------------------------------------------------------------
  
  /** Convert an IRectangle into a string for logging or display
   * 
   * @param rect the IRectangle to convert
   * 
   * @return a string representation of the rectangle.
   */
  public static final String toString(IRectangle rect) {
    return Point.toString(rect) + ", " + Dimension.toString(rect);
    }
  
  }
