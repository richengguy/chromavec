================
Colour Gradients
================

Something that we "lose" when moving to colour is the notion of image gradients.
Gradients tell us how much something is changing and the direction in which its
changing.  The actual concept comes from calculus and this is what you usually
see applied in image processing when discussing image gradients.  The purpose
of this section is to describe how you can construct gradients (sort of) for
colour images that can then be used with pre-existing edge detectors, such as
the `Canny edge detector <https://en.wikipedia.org/wiki/Canny_edge_detector>`_
[canny1986]_.

I'll also note that the contents of this section are based off of the work of
Scharcanski and Venetsanopolous [scharcanski1997]_.  The main difference is that
it has a few simplifications to (hopefully) make *understanding* what's going on
a bit easier.  It also handles the gradient angle estimation a bit differently.
Anyway, the point is that you can greyscale-style filtering with colour images
with only a few simple tweaks to the original algorithm.

Gradients 101
=============

First, let's go over what we mean by "image gradients", starting from first
principles whenever possible.  Unfortunately, especially in academic literature,
this tends to be described using the language of calculus which can appear a bit
dense at times.  Let's avoid that for the time being and just look at what's
actually going on with images before introducing any calculus.

Simple Example
--------------

Let's start off with a simple example consisting of a 3x3 "image", like the one
below:

.. math::

    \begin{array}{|c|c|c|}
        \hline
        255 & 255 & 255 \\
        \hline
        0 & 0 & 255 \\
        \hline
        0 & 0 & 255 \\
        \hline
    \end{array}

This is pretty similar to the example from the
:doc:`previous section <vector-order-statistics>`.  In that section, there was
the following filter kernel:

.. math::

    h[i,j] = \begin{bmatrix} 1 & -1 & 0 \end{bmatrix}.

Well, this just also happens to be the filter kernel that describes the
**forward difference** of a discrete sequence.  Then for an image, horizontal
central difference :math:`\Delta_x I[x,y]` is given by

.. math::

    \Delta_x I[x,y] = I[x+1,y] - I[x,y].

There's also (surprise, surprise) a corresponding vertical central difference

.. math::

    \Delta_y I[x,y] = I[x,y+1] - I[x,y].

If we apply the two operators onto the *same* image we'll get *two* new images,
which I'll call :math:`G_x` and :math:`G_y` for the horizontal and vertical
differences:

.. math::

    G_x = \begin{array}{|c|c|c|}
        \hline
        0 & 0 & 0 \\
        \hline
        0 & 255 & 0 \\
        \hline
        0 & 255 & 0 \\
        \hline
    \end{array}

and

.. math::

    G_y = \begin{array}{|c|c|c|}
        \hline
        0 & 0 & 0 \\
        \hline
        255 & 255 & 0 \\
        \hline
        0 & 0 & 0 \\
        \hline
    \end{array}.

What exactly did that do?  Notice that :math:`G_x` tells us where there's a
*vertical* transition while :math:`G_y` does the same for horizontal
transitions.  That means that in order to properly capture edges, we need to
look in *at least* two directions, i.e. horizontal and vertical [#f1]_.

Direction and Magnitude
-----------------------

This is where the connection to calculus starts to come in.  The two images,
:math:`G_x` and :math:`G_y` can be thought of, loosely [#f2]_, as partial
derivatives.  It's why some sources will use

.. math::

    \frac{\partial I}{\partial x}

and

.. math::

    \frac{\partial I}{\partial y}

to refer to :math:`G_x` and :math:`G_y`.  In vector calculus, the gradient, or
direction of change, for a function :math:`f(x,y)` is defined as

.. math::

    \Delta f = \left( \frac{\partial f}{\partial x}, \frac{\partial f}{\partial y} \right)

So, this idea gets used in image processing to define image gradients, i.e.

.. math::

    \Delta I = \left( G_x, G_y \right).

That means that for a greyscale image, its gradient is a two-colour image, where
one colour contains the horizontal gradients and the other contains the vertical
gradients.  The strength of each edge, its magnitude, is

.. math::

    \| \Delta I \| = \sqrt{ G_x^2 + G_y^2 }

while the gradient angle [#f3]_ is

.. math::

    \angle \Delta I = \arctan\left( \frac{G_y}{G_x} \right).

If you compute the magnitude and angles for the 3x3 example image then you
get

.. math::

    \| \Delta I \| = \begin{array}{|c|c|c|}
        \hline
        0 & 0 & 0 \\
        \hline
        255 & 360 & 0 \\
        \hline
        0 & 255 & 0 \\
        \hline
    \end{array}

.. math::

    \angle \Delta I = \begin{array}{|c|c|c|}
        \hline
        0^\circ & 0^\circ & 0^\circ \\
        \hline
        90^\circ & 45^\circ & 0^\circ \\
        \hline
        0^\circ & 0^\circ & 0^\circ \\
        \hline
    \end{array}.

With this simple operation, we know a) if there's an edge and b) which direction
it points.  And this leads to...

Canny-style Edge Detection
--------------------------

The Canny edge detector [canny1986]_ is **the** edge detector in classic
computer vision and image processing.  It does a very good job at isolating
edges in an image and is pretty much the goto edge detector for many
applications.  It can be broken into the following stages:

1. Pre-filtering (blurring) an image to remove noise.
2. Compute the horizontal and vertical gradient images, :math:`G_x` and
   :math:`G_y` using an edge detection kernel.
3. Non-maximum suppression (NMS) to set anything not right *on* a potential edge
   to zero.
4. Hysteresis thresholding, where all candidate edges are broken into strong and
   weak edges based on their magnitudes.
5. Connected component analysis (or equivalent) to join weak edges to strong
   edges.

We'll mainly focus on the second step since that's what we need to modify to get
the Canny edge detector working with colour.  A good introduction to the
detector can be found at `AI Shack <http://www.aishack.in>`_:

* `Introducing the Edge Detector <http://www.aishack.in/tutorials/canny-edge-detector/>`_
* `Implementing Canny Edges from scratch <http://www.aishack.in/tutorials/implementing-canny-edges-scratch/>`_


Working with Colour
===================

As hinted, the main change that needs to be made to the Canny edge detector so
that it works with colour is in the second step when you calculate the gradient
directions.  The reason has to do with one particular fact about vector norms:
they're always positive.  That throws certain assumptions about gradient
*angles* out the window.

Another Example
---------------

Let's consider the original image example except inverted (:math:`0 \to 255` and
vice-versa):

.. math::

    \begin{array}{|c|c|c|}
        \hline
        0 & 0 & 0 \\
        \hline
        255 & 255 & 0 \\
        \hline
        255 & 255 & 0 \\
        \hline
    \end{array}.

That means the gradient images are

.. math::

    G_x = \begin{array}{|c|c|c|}
        \hline
        0 & 0 & 0 \\
        \hline
        0 & -255 & 0 \\
        \hline
        0 & -255 & 0 \\
        \hline
    \end{array}

and

.. math::

    G_y = \begin{array}{|c|c|c|}
        \hline
        0 & 0 & 0 \\
        \hline
        -255 & -255 & 0 \\
        \hline
        0 & 0 & 0 \\
        \hline
    \end{array}.

The magnitudes are the same but the *angles* point in the opposite direction,
as seen below:

.. math::

    \| \Delta I \| = \begin{array}{|c|c|c|}
        \hline
        0 & 0 & 0 \\
        \hline
        255 & 360 & 0 \\
        \hline
        0 & 255 & 0 \\
        \hline
    \end{array}

.. math::

    \angle \Delta I = \begin{array}{|c|c|c|}
        \hline
        0^\circ & 0^\circ & 0^\circ \\
        \hline
        -90^\circ & -135^\circ & 0^\circ \\
        \hline
        0^\circ & 180^\circ & 0^\circ \\
        \hline
    \end{array}.

Basically, when we inverted the image, the angles also flipped by 180-degrees
to accomodate.  Because the angles are flipped, the detector can still do the
non-maximum suppression correctly because it knows what direction is
perpendicular to an edge.

Colour Angles
-------------

Here's where things get tricky.  First, remember that the goal is to figure out
the strength of an edge (i.e. magnitude) as well as its orientation (angle).  As
long as we can do that, we can use the Canny edge detector.

Let's now look at a colour example, similar to the original greyscale example
but with yellow and red instead of black and white.

.. math::

    \begin{array}{|c|c|c|}
        \hline
        (255, 0, 0) & (255, 0, 0) & (255, 0, 0) \\
        \hline
        (255, 255, 0) & (255, 255, 0) & (255, 0, 0) \\
        \hline
        (255, 255, 0) & (255, 255, 0) & (255, 0, 0) \\
        \hline
    \end{array}

If we apply the normal difference filters, we'll get the follow two images:

.. math::

    G_x = \begin{array}{|c|c|c|}
        \hline
        (0, 0, 0) & (0, 0, 0) & (0, 0, 0) \\
        \hline
        (0, 0, 0) & (0, -255, 0) & (0, 0, 0) \\
        \hline
        (0, 0, 0) & (0, -255, 0) & (0, 0, 0) \\
        \hline
    \end{array}

.. math::

    G_y = \begin{array}{|c|c|c|}
        \hline
        (0, 0, 0) & (0, 0, 0) & (0, 0, 0) \\
        \hline
        (0, -255, 0) & (0, -255, 0) & (0, 0, 0) \\
        \hline
        (0, 0, 0) & (0, 0, 0) & (0, 0, 0) \\
        \hline
    \end{array}.

Remember how we computed the magnitude as :math:`\sqrt{G_x^2 + G_y^2}` and the
angle as :math:`\arctan\left(\frac{G_y}{G_x}\right)`?  Well, we can't do that
any more.  However, when we look at the image, we know, for lack of a better
term, that you've got a horizontal edge (0-degrees) and a vertical edge
(90-degrees).  We also know that the jump from one colour to another is
:math:`(0, -255, 0)`.

Well, this starts to look a little bit like the vector order statistics that
we looked at :doc:`before <vector-order-statistics>`.  It's not unreasonable to
say that the size of the jump is :math:`d\bigl((255, 255, 0), (255, 0, 0)\bigr) = 255`.
So, that's a good starting point.  We can now generate a gradient magnitude
image :math:`\| \Delta I \|` for colour images.

To make things a bit more concrete, consider the following image:

.. figure:: img/colgrad/elgin-cropped.jpg

    Example image.

Let's apply the following operation

.. math::

    \| \Delta I \| = \max\left(d(G_x), d(G_y)\right)

to all pixels.  All that we're doing is taking the maximum response between the
horizontal and vertical differences.  This is what the output looks like:

.. figure:: img/colgrad/elgin-grad-mag.jpg

    Colour gradient magnitudes.

So far so good, but what about the angle?  We lose the sign when taking the
vector norm, because, **by definition**

.. math::

    d(\vec{x}) \ge 0 \ \forall \ \vec{x}.

That means that we don't know if the edge is, for example, from left-to-right or
right-to-left.

Is this a problem?  Nope!  We don't actually care which direction that edge
arrow points.  As long as its 90-degrees to the edge, who cares?  In fact, this
is the same assumption that the Canny detector's NMS step makes.  The way around
this is to sample around a pixel in *multiple* steps.  Specifically, this
pattern, in this order

.. math::

    0^\circ = \begin{array}{|c|c|c|}
        \hline
        \  & \  & \  \\
        \hline
        - & \star & + \\
        \hline
        \  & \  & \  \\
        \hline
    \end{array}

.. math::

    90^\circ = \begin{array}{|c|c|c|}
        \hline
        \  & + & \  \\
        \hline
        \  & \star & \  \\
        \hline
        \  & -  & \  \\
        \hline
    \end{array}

.. math::

    45^\circ = \begin{array}{|c|c|c|}
        \hline
        \  & \     & + \\
        \hline
        \  & \star & \  \\
        \hline
        - &  \     & \  \\
        \hline
    \end{array}

.. math::

    135^\circ = \begin{array}{|c|c|c|}
        \hline
        - & \     & \  \\
        \hline
        \  & \star & \  \\
        \hline
        \  & \     & + \\
        \hline
    \end{array}

The final gradient, magnitude **and** direction, is formally defined as

.. math::

    \Delta I = \arg \max_{\theta} \left\{ d(\vec{c}_+, \vec{c}_-) \right\},

where

.. math::

    \theta = \left\{ 0, 45, 90, 135 \right\}

and :math:`\vec{c}_+` and :math:`\vec{c}_-` are the pixel colours sampled at
the :math:`+` and :math:`-` positions, respectively.  This is basically a form
of the Vector Range filter except that it's not a rectangular window.  The
reason for the sampling order is that even though there are eight-possible
angles, four of them are just reflections of the other.  Because we loose the
sign, we don't have to compute these twice.

When you apply this filter, what get looks something like this:

.. figure:: img/colgrad/elgin-grad.jpg

    Colour vector gradients encoded using an HSV colour scheme.  The brightness
    corresponds to the magnitude while the colour corresponds to the angle.

Closing the Loop
----------------

Once we have colour gradients, in the same form expected by the Canny edge
detector, we're mostly done.  The main change, at least in the implementation
accompanying this document, is that the gradients are stored in polar, or
magnitude-angle, form rather than in :math:`(\partial x, \partial y)` form.
This is a fairly minor change and mainly to simplify the overall implementation.


If this were to be used with another implementation, e.g. OpenCV's
`Canny() function <https://docs.opencv.org/3.4.2/dd/d1a/group__imgproc__feature.html#ga2a671611e104c093843d7b7fc46d24af>`_,
then you can convert to the Cartesian form by

.. math::

    \partial x &= \| \Delta I \| \cos \left( \angle \Delta I \right)

    \partial y &= \| \Delta I \| \sin \left( \angle \Delta I \right),

which is a standard polar-to-rectangular conversion.  The final result of the
colour-augmented Canny edge detector is

.. figure:: img/colgrad/elgin-edges.png

    Resulting edge map from the previously generated colour gradients.  The
    detector settings were :math:`\sigma = 2` and
    :math:`th = \left[10, 20 \right]`.

Other Methods
=============

As mentioned at the start, there has already been work into looking at
determining edges from colour images.  There's the vector order statistics
work by Lukac et al. [lukac2005]_ and discussed in-depth in the
:doc:`vector-order-statistics` section.  As well, there is work by
Scharcanski and Venetsanopolous [scharcanski1997]_ which was the basis for this
section.

There's also the work by Ruzon and Tomasi [ruzon2001]_ on finding various
low-level image features (edge, t-junctions and corners) all using the same
colour-based framework.  They do something similar to what was described in this
section, sampling various angles around a pixel but they use a completely
different distance measure.  Rather than using a vector norm like Euclidean
distance, they instead use something called
`Earth Mover's Distance <https://en.wikipedia.org/wiki/Earth_mover%27s_distance>`_
(EMD).  EMD measures how much "effort" is required to move between two
distributions.  The idea is a bit abstract but one way to think about it is if
you have a bunch of red-ish pixels and a bunch of green-ish pixels, it's the
amount of effort to go from red to green.


Another approach, which was developed by Kypriandis and Dollner
[kyprianidis2008]_, was used for image stylization.  Their approach was based in
vector calculus so that the partial derivative of a colour image was defined as

.. math::

    \frac{\partial I}{\partial x} = \begin{bmatrix}
        \frac{\partial R}{\partial x} &
        \frac{\partial G}{\partial x} &
        \frac{\partial B}{\partial x}
    \end{bmatrix}^T

for the horizontal derivative and

.. math::

    \frac{\partial I}{\partial y} = \begin{bmatrix}
        \frac{\partial R}{\partial y} &
        \frac{\partial G}{\partial y} &
        \frac{\partial B}{\partial y}
    \end{bmatrix}^T

for the vertical.  This allows them to construct something called a structure
tensor [dizenzo1986]_ which can be decomposed to find the directions of an edge.

.. rubric:: Footnotes

.. [#f1] More advanced schemes use something called "steerable filters" where,
         among other things, you can adjust the angle of the edge detector.
         This provides better resolution of edges that may not be near vertical
         or horizontal angles.

.. [#f2] I use the term "loosely" because partial derivatives are defined for
         *continuous* multi-dimensional functions, which is what calculus is
         meant to work with.  Images are defined on a discrete lattice (i.e.
         grid) so calculus doesn't *quite* apply. Instead, a lot of operations
         are more naturally defined in terms of linear algebra.

.. [#f3] This is usually calculated using the `atan2() function <https://en.wikipedia.org/wiki/Atan2>`_
         since it ensures that the angle is calculated correctly.
