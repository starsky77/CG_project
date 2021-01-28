

# 虚拟博物馆设计说明文档

## 1.功能描述

本项目基于OpenGL，通过GLAD，GLFW访问OpenGL规范接口，自主实现一个简易的图形引擎，来模拟一个虚拟博物馆，并场景进行漫游与编辑。本项目实现了如下功能：

+ **通过鼠标与键盘对场景进行任意方向的自由漫游；**

+ **实时的光照以及阴影效果，可以移动光源位置，并能够实时更新阴影以及光照的效果；**
+ **对物体纹理贴图的显示，其中也包括漫反射贴图/镜面反射贴图这类光照贴图**；
+ **使用非真实质感的shader对场景进行渲染；**
+ **mesh模型，天空盒的载入以及显示；**
+ **NURBS曲面的建模**；
+ **L-Tree的建模；**
+ **通过鼠标中间选择特定物体（包括自定义的简单体素（如立方体），mesh模型，L-Tree，NUBRS曲面），并放置于任意位置；**
+ **通过鼠标选中任意物体，并拖动至任意位置；**

+ **在安卓移动平台上实现了漫游功能，并加入了VR效果；**



## 2 项目依赖以及文件结构说明

**若需要编译运行本项目源代码，需要如下依赖：**

+ OpenGL接口GLAD以及GLFW
+ OpenGL数学函数库GLM
+ 读取模型需要assimp库
+ 纹理图像的读取需要stb_image.h，可以加入stb_image.h到工程中，或是安装stb库

**工程文件目录如下：**

+ mod中存储相关模型文件

+ skybox中装有天空盒的图片资源

+ shaders中储存所有shader的代码集合

+ nurbs中包含NURBS相关的源文件

+ 工程文件根目录下还有一些纹理图片素材，请勿删除





## 3.系统结构设计

### 3.1 程序整体架构

工程文件的结构如下

![](./img/1.png)

其中：

+ NURBS系统实现于`nurbs`以及`nurbs_include`文件夹内的源文件；
+ L-system实现于`Grammar.h`，`Grammar.cpp`，`Parse.h`，`Parse.cpp`中
+ 模型的导入实现于`mesh.h`以及`model.h`中
+ 光照系统实现于`light.h`,`light.cpp`中，同时在相应的shader代码中包含了关于光照的计算。
+ shader的载入以及使用集成在`shader.h`中,纹理实现在shader中
+ 相机系统实现于`camera.h`中
+ 鼠标拾取，阴影绘制和主循环的耦合较高，故在`main.cpp`中实现
+ `utils.h`中包含着各种绘制函数，各类初始化函数，在`main.cpp`中被使用
+ `Object.h`中实现了多物体管理系统，是通过多叉树来管理场景内的所有新增物体



### 3.2 NUBRS曲面建模

#### 3.2.1 理论基础

##### 3.2.1.1 B-spline curve

B-spline curve的定义：
$$
\mathbf{C}(u)=\sum_{i=0}^{n} N_{i, p}(u) \mathbf{P}_{i} \quad a \leq u \leq b
$$
其中$\{P_i\}$是所有的控制点的集合。$\{N_{i, p}(u)\}$是B-spline的p阶基函数，它的定义如下：
$$
\begin{array}{l}
N_{i, 0}(u)=\left\{\begin{array}{ll}
1 & \text { if } u_{i} \leq u<u_{i+1} \\
0 & \text { otherwise }
\end{array}\right. \\
N_{i, p}(u)=\frac{u-u_{i}}{u_{i+p}-u_{i}} N_{i, p-1}(u)+\frac{u_{i+p+1}-u}{u_{i+p+1}-u_{i+1}} N_{i+1, p-1}(u)
\end{array}
$$
其中u是一串**非递减**的值，被称为**knot**，我感觉翻译过来可以是节点，但是个人的理解是参数u的插值点（注意不是真的插值点，因为插值点是在三维平面是三维的，而u是一维的，也就是$ \{u\}$是一个$1\times (m+1)$维的向量）。$\{u\}$定义如下（$u_i\leq u_j$）：
$$
U=\{\underbrace{a, \ldots, a}_{p+1}, u_{p+1}, \ldots, u_{m-p-1}, \underbrace{b, \ldots, b}_{p+1}\}
$$
在本次pro中，采用a=0，b=1。



##### 3.2.1.2 B-spline surface

B-spline surface的定义：
$$
\mathbf{S}(u, v)=\sum_{i=0}^{n} \sum_{j=0}^{m} N_{i, p}(u) N_{j, q}(v) \mathbf{P}_{i, j}
$$
该定义就是对B-spline curve的二元拓展，参照前面即可，不再赘述。



##### 3.2.1.3 NURBS curve

NURBS curve的定义：
$$
\mathbf{C}(u)=\frac{\sum_{i=0}^{n} N_{i, p}(u) w_{i} \mathbf{P}_{i}}{\sum_{i=0}^{n} N_{i, p}(u) w_{i}} \quad a \leq u \leq b
$$
其中$\{w_i\}$是权重因子，其他的符号定义与B-spline curve一致。



##### 3.2.1.4 NURBS surface

NURBS surface就是在NURBS curve上的二元拓展：
$$
\mathbf{S}(u, v)=\frac{\sum_{i=0}^{n} \sum_{j=0}^{m} N_{i, p}(u) N_{j, q}(v) w_{i, j} \mathbf{P}_{i, j}}{\sum_{i=0}^{n} \sum_{j=0}^{m} N_{i, p}(u) N_{j, q}(v) w_{i, j}} \quad 0 \leq u, v \leq 1
$$
$\{\mathbf{P}_{i, j}\}$是二元的控制点，相当于一张控制网。$\{w_{i, j}\}$是权重，$\{N_{i, p}(u)\}\{N_{j, q}(v)\}$是分别以u,v为参数的基函数。其中u,v两个参数是定义在U,V两个knot vector上。
$$
\begin{array}{c}
U=\{\underbrace{0, \ldots, 0}_{p+1}, u_{p+1}, \ldots, u_{r-p-1}, \underbrace{1, \ldots, 1}_{p+1}\} \\
V=\{\underbrace{0, \ldots, 0}_{q+1}, v_{q+1}, \ldots, v_{s-q-1}, \underbrace{1, \ldots, 1}_{q+1}\}
\end{array}
$$

$$
r=n+p+1 \text { and } s=m+q+1
$$

NURBS在我的理解中感觉就是两个NURBS曲线在u, v两个方向的叠加，从而张成的曲面，由于权重的存在，使得NURBS更加灵活。



#### 3.2.2 编程实现

##### 3.2.2.1 寻找节点所在段

要带入参数u需要计算它在knot vector上的位置，也就是它在分段的位置，**不同的分段的基函数是不一样的**。找到相应的区间，才能继续计算下次。本次采用是的是非常简单的**二分**的方法。

```c++
int findSpan(int n, int p, double u, vector<double> U)
{
	if (u == U[n + 1])
		return n;

	int low = p;
	int high = n + 1;
	int mid = (low + high) / 2;

	while (u < U[mid] || u >= U[mid + 1])
	{
		if (u < U[mid])
			high = mid;
		else
			low = mid;
		mid = (low + high) / 2;
	}

	return mid;
}
```



##### 3.2.2.2 计算基函数

定义：
$$
\operatorname{left}[j]=u-u_{i+1-j} \quad \text { right }[j]=u_{i+j}-u
$$
计算基函数：

```c++
void basisFuns(int i, double u, int p, vector<double> U, vector<double> &N )
{
	assert( N.size() > 0 );

	N[0] = 1.0;
	vector<double> left(p+1);   // only the last p elements are used
	vector<double> right(p+1);

	for (int j = 1; j <= p; j++)
	{
		left[j] = u - U[i + 1 - j];
		right[j] = U[i + j] - u;
		double saved = 0;

		for (int r = 0; r < j; r++)
		{
			double temp = N[r] / (right[r + 1] + left[j - r]);
			N[r] = saved + right[r + 1] * temp;
			saved = left[j - r] * temp;
		}
		N[j] = saved;
	}
}
```



##### 3.2.2.3 计算surface point

通过结合findSpan和basisFuns两个函数，再给定之前公式中的各个参数就可以就算出NURBS曲面上的某一点，各个参数包括控制点序列$\{P\}$, 两个knot vector--$\{U\},\{V\}$， 两种基函数的阶数p, q。控制点的个数n,m。以及参数变量u,v。

```c++
Point3D getNURBSSufacePoint(int n, int p, vector<double> U, int m, int q, vector<double> V, vector<vector<Point4D>> P, double u, double v)
{
	vector<double> Nu(p + 1);
	vector<double> Nv(q + 1);

	int uSpan = findSpan(n, p, u, U);
	basisFuns(uSpan, u, p, U, Nu);
	int vSpan = findSpan(m, q, v, V);
	basisFuns(vSpan, v, q, V, Nv);

	vector<Point4D> temp;
	temp.resize(q + 1);
	for (int l = 0; l <= q; l++)
	{
		for (int k = 0; k <= p; k++)
			temp[l] = temp[l] + P[uSpan - p + k][vSpan - q + l] * Nu[k];
	}
	Point4D Sw;
	Point3D S;
	for (int l = 0; l <= q; l++) {
		Sw = Sw + temp[l] * Nv[l];
	}
	Sw = Sw / Sw.getW();
	return Point3D(Sw.getX(), Sw.getY(), Sw.getZ());
}
```



#### 3.2.3 结合GLAD

因为本次大程的时候采用的是线代Opengl编程，都是通过shader渲染。画物体都是通过三角面片或者线段等最基础的方式来实现，而不能使用glVertex相关的函数。因此算出所有的surface上的点后如何把它画出来也是比较重要的一步。

#### 3.2.3.1 绑定VBO

因为我的点是存在三维数组（一个三维坐标的一维数组）里面，还得把三维数组变换成一维数组，然后绑定VBO。

#### 3.2.3.2 绑定EBO

因为我算出来的曲面的点是不重复的，所以需要有index来引导GPU根据不重复的点来重复使用这些点从而画出整个曲面。

本来我生成的是一张三维的网。每个网格都是曲面的四边形面片。现在把它拆成三角形，并生成相应的index。我是把四边形拆成了ABCD拆成了ABD + BCD。

```c++
	for (int i = 0; i < row - 1; i++) { // row
		for (int j = 0; j < column - 1; j++) { // column
			// ABCD -> ABD + BCD
			indices[n++] = Index3(i * column + j, i * column + j + 1, (i + 1) * column + j);
			indices[n++] = Index3(i * column + j + 1, (i + 1) * column + j, (i + 1) * column + j + 1);
		}
	}
```



#### 3.2.3.3 绑定VAO

方便管理和下次重画。

### 3.3 L-System

#### 3.3.1L-System原理

- L－系统是匈牙利生物学家Aristid LinderMayer于1968年提出的。L－系统的本质是一个重写系统，通过对植物对象生长过程的经验式概括和抽象，初始状态与描述规则，进行有限次迭代，生成字符发展序列以表现植物的拓扑结构，并对产生的字符串进行几何解释，就能生成非常复杂的分形图形。
- 通过上述描述，我们可以看出L-system的本质其实是一个递归生成的文法，由一个最基本的“Axiom”经过rules的替换后生成一串文法，然后根据这个文法，我们通过解析文法中的字符，在opengl中做出相对应的操作，完成L-system的绘制
- L-system通常用在“树”的建模上，目前由L-system建树是一个较为成熟的方案，我们通过修改文法生成规则以及文法的绘制基本图元等，就可以自定义生成出各种各样不同类型的树种。同时，我们可以修改递归层数，以此确定树的成长状况、数目的繁茂程度和大小等等。
- 此外L-system的递归思路可以演示出一棵树由小到大的一个基本过程，通过将每一个枝干上通过同等的操作替换生成新的小枝桠，即可展示出树从一根小树苗生长的模拟过程。由此可见，L-system还是非常具有实用性的程序，所以本次我选择实现一个不在期末大程要求中的L-system作为我们图形学小组中建模部分的模型。

#### 2.3.2L-System实现

- 代码结构：

  - 本次工程共包含两个自定义类，Grammar与Parse。分别对应一个*.h *.cpp文件
  - Grammar作为文法类，主要目的是实现用户自定义递归文法，主要任务就是递归通过rules生成一个最终结果的String。
  - Parse作为解析类，主要任务是完成对文法类生成的string进行解析，并于opengl相结合，绘制出对应字符要求的操作。
  - 在本次实验中，我的文法类于解析类封装的较好，并且有丰富的用户DIY自由度，用户可以通过修改参数和规则生成自己满意的Ltree。

- Grammar实现分析

  - Grammar包含两个类，Tuple类作为一个便于保存递归rules的子类，是Grammar类的一个基层实现。

    Grammar类中Axiom是初始的字符，表示递归的起点，vector是对递归规则的保存，采用了Tuple类，由键值、替换规则唯一确定。同一个键值可以有不同的个替换规则，在替换时我们优先替换在vector前方的替换规则。Level表示递归层数，Result代表最后递归结束后生成的结果string。

  ```c++
  #ifndef _GRAMMAR_H
  #define _GRAMMAR_H
  
  #include<string>
  #include<vector>
  #include<iostream>
  using namespace std;
  
  class Tuple
  {
  public:
  	char key;						// left
  	string t;						// right
  
  	Tuple() {}
  	Tuple(char c, string st) :key(c), t(st) {}
  
  	void print() { cout << "key: " << key << "\t" << "to: " << t; }
  };
  
  class Grammar
  {
  private:
  	string Axiom;					// start string
  	vector<Tuple> Generation;		// generation rules
  	int Level;						// generation level
  	string Result;					// result of generations
  
  public:
  	Grammar() {}					// default cotr
  	Grammar(string axiom, vector<Tuple> generation, int level) :Axiom(axiom), Generation(generation), Level(level) {
  		Result = Axiom;				// init result
  		Generate();					// generate result
  		cout << "Create Grammar success!" << endl;
  		print();					// print information
  	}
  
  	void Generate();				// generate result
  	void print();					// print information of Grammar
  
  	string getResult() { return Result; }
  
  	void defaultinit();
  };
  
  #endif // !_GRAMMAR_H
  ```

  - 关于类内实现方法，主要是Generate（）函数。

    下文给出了generate的源码，主要部分包含了三层循环，分别代表递归层数循环、字符串长度循环、递归规则遍历循环。值得一提的是，我们每一曾递归将会把本层递归结果作为result写入，并从下一曾递归开始时，设置result作为当前层的入口，进行string中对应键值的替换。从最内层的遍历替换规则循环中可以看出，我们采用顺序遍历方式，遍历rules。

    难点在于处理string的总长度和当前正在处理的位置，要防止重复处理同一层中已替换过的字符。

  ```c++
  void Grammar::Generate()
  {
  	string res = Result;					// init resz
  	int	num = Generation.size();			// num of replace rules
  
  	for (int i = 0; i < Level; i++) {
  		// process i Level replacement
  
  		for (unsigned int len = 0; len < res.length(); len++) {
  			// each time we process a char in res and check replace
  
  			for (int nnum = 0; nnum < num; nnum++) {
  				// check for each replace tule
  
  				if (res[len] == Generation[nnum].key) {
  					// if this char can be replaced
  
  					res.replace(len, 1, Generation[nnum].t);
  					len += Generation[nnum].t.length() - 1;	// we need to change current length to avoid replace sub-level char
  				}
  				else {
  					// if this char can't be replaced
  
  					continue;
  				}
  			}
  		}
  	}
  	Result = res;
  }
  ```

  

- Parse实现分析

  - 于Grammar类相似，Parse.h也定义了两个类，分别是tmp与Parse，其中tmp作为struct的形式呈现
  - tmp的数据结构主要是保存在递归生成树时的堆栈内容数据结构，包含当前画笔位置、当前画笔法向、当前树木粗细程度三部分

  ```c++
  struct Tmp {							// we use a class Tmp to store information in recursion
  public:
  	vector<float> location;				// current location x,y,z | default to (0,0,0)
  	vector<float> direction;			// current grow direction | default to (0,1,0)
  	float linewidth;					// trunk width
  };
  ```

  - Parse类则内嵌了文法类生成的结果字符串input、内置了递归时所需的栈mystack，以上文中的tmp作为数据结构。此外还包括对数目的具体要求数据，例如长度、宽度、旋转角等

  ```c++
  class Parse{
  private:	
  	string input;
  	const vector<char> alpha = { 'F','f','+','-','\\','/','^','&','[',']' };
  
  	stack<Tmp> mystack;
  
  	float length;						// tree trunk length
  	float width;						// tree trunk width
  	float rotateX;						// rotate x degree
  	float rotateY;						// rotate y degree
  	float rotateZ;						// rotate z degree	
  
  public:
  	Parse(){}
  	Parse(string s, float l, float w, float x, float y, float z) :input(s),length(l),width(w),rotateX(x),rotateY(y),rotateZ(z) {}
  	
  	void ParseDefault();				// create a default Parse test demo
  	void DrawLSystem();					// draw L-system based tree
  
  private:
  	void drawLine();					// for draw the trunk in recursion
  
  	void RotateX(int check);			// rotate x function
  	void RotateY(int check);			// rotate y function
  	void RotateZ(int check);			// rotate z function
  
  	void push();						// push stack
  	void pop();							// pop stack
  
  	void Parseinit();					// initialization of stack and shader
  };
  
  #endif // !_PARSE_H
  
  ```

  - 关于Parse类内函数的实现，主要通过两个函数drawLine（）和DrawLSystem（）。其中，DrawLSystem是提供给用户的接口。在DrawLSystem中，通过遍历string，得到每个对应键值的操作函数，进行绘制操作。
  - 在drawLine函数中，我们定义了一个树木基本图元的绘制结构。为了方便表示与后期转化成定点矩阵保存编辑，我们采用了长方体的形式。其中vertices就是表示当前字符所对应的图元。它通过查看栈顶的数据，得到当前图元的位置、大小、方向等参数，绘制Ltree的一个图元。

  ```c++
  void Parse::drawLine()
  {
  	float nowx = mystack.top().location[0];
  	float nowy = mystack.top().location[1];
  	float nowz = mystack.top().location[2];
  	float nextx = mystack.top().direction[0] * length;
  	float nexty = mystack.top().direction[1] * length;
  	float nextz = mystack.top().direction[2] * length;
  
  	float line = mystack.top().linewidth;
  
  	static float v[] = {
  		// positions														// normals           // texture coords
  		// back 
  		nowx,					nowy,				nowz,					0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
  		line + nowx,			nowy,				nowz,					0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
  		line + nowx + nextx,	nowy + nexty,		nowz + nextz,			0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
  		line + nowx + nextx,	nowy + nexty,		nowz + nextz,			0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
  		nowx + nextx,			nowy + nexty,		nowz + nextz,			0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
  		nowx,					nowy,				nowz,					0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
  		// front
  		***
  		// left
  		***
  		// right
  		***
  		// down
  		***
  		// up
  		***
  	};
  
  	float *vertices = new float[288];			// 288=8*6*6
  	for (int i = 0; i < 288; i++) {
  		vertices[i] = v[i];
  	}
  	vec.push_back(vertices);
  
  	// reset mystack
  	mystack.top().linewidth -= 0.1;
  	mystack.top().location = { nowx + nextx,nowy + nexty,nowz + nextz };
  }
  ```

  - 在Parse中，我们还定义了堆栈的push、pop操作，以及对图元的旋转操作，在此不再赘述。
  - 特别注意，笔者的旋转并不是标准的旋转，实现与坐标轴旋转有明显区别，所以效果可能会有较大出入。

### 3.4 鼠标拾取

opengl4.0开始就取消了GL_SELECT的绘制模式。鼠标拾取需要自己造轮子。不过也有好处，就是拾取的那一趟绘制不是白白浪费，在反馈选中信息的同时也可以在屏幕上输出。
鼠标拾取要传入3个点的坐标并判断鼠标位置是否位于组成的三角形内。这个工作无法再vertex shader(逐顶点)或者fragment shader(已经光栅化)中完成，因此要引入新的流水线阶段geometry shader。判断的过程就是：

1. 使用uniform传入鼠标的坐标；
2. vertex shader传入的顶点坐标的xy分量分别除以w分量得到屏幕上面片的顶点三角形的坐标；
3. 用重心坐标鼠标选中点。如果线性组合的三个系数都大于0则面片被选中。

在选中面片基础上实现选中整个物体也稍麻烦，因为此时shader要为主程序提供反馈告知选中的物体名，两者无可避免地需要共享一段内存并引入一定的开销。bufferImage 的功能能够满足需要，可以创建一个随机访问、在主机和shader之间共享的内存缓冲。
调用使用的代码如下：

最终得到的选中效果在敏感和精确两方面都无可挑剔。
为了增强显示的效果，表明选中的是整个物体而不是一个面片，还为选中的物体增加绘制了边缘。这一部分比较容易，主要分两次绘制。
第一趟把天空盒和地面的模板设置为0，其他物体的模板设置为0xff。
第二趟只用设置模板函数为：绘制的时候绘制条件设置成notEqual。在原帧的基础上绘制一个黑色的稍大的被选中物体即可。

### 3.5 实时阴影绘制

本次只实现了单光源的实时硬阴影,shadow map方式实现。实时阴影也需要两次绘制。第一次生成一个从光源角度看的深度图，第二次在摄像机视角绘制到屏幕；绘制中决定像素点颜色时要将绘制的像素点转移到光源视角的坐标系下并和深度图比较。若深度图上同位置的像素深度更小，则该像素点处在阴影中。

使用的代码如下：

我在debug模式下将深度图缩略显示在右上角，可以很容易判断第一阶段的生成的深度图的正确性。

#### 待改进：

由于第一趟绘制深度图时目前采用了固定的帧大小，导致窗口大小调整的时候不能保证阴影的正确性。

### 3.6 相机封装：

相机类维护了front, up, right(都是单位向量), position向量。通过这几个向量可以很方便地使用lookAt函数得到相机变换矩阵。

### 3.7 初步的建模模式

建模模式中我编写的部分主要是将待放置的物体绘制在相机前方，并与相机的方位移动绑定。主要是需要合理的设置传入shader的model矩阵。

确定物体的model矩阵的流程如下：

1. 先获得原点与世界坐标系原点重合、坐标轴与相机坐标系平行的中间坐标系。世界坐标系到这个坐标系的过渡矩阵是将camera.x,camera.y,camera.z三个单位向量并在一起组成的矩阵的转置。
2. 获得世界坐标系中的物体位置；只需要用相机的位置再加上相机的前方向，而这两个数据都在Camera类中封装。
3. 将这个平移辆转换为物体上述坐标系下的平移量。将这个向量分别与三个坐标方向点积即可。
   具体的代码实现如下：

 ```
   model = glm::mat4(glm::mat3(camera.Right, camera.Up, -camera.Front));
   model = glm::translate(model, camera.Position * glm::mat3(model) + glm::vec3(0.0, 0.0, -3.0));
 ```



### 3.8 （多）光源系统

项目中期节点实现了多光源系统，能够建立多个点光源，并可以分别体现出点光源、平行光、聚光的效果。但是由于阴影模块仅实现了单光源的阴影，故为了配合单光源阴影，实际工程中展示的仅有单光源的效果，不过相应的代码均封装于`light.h`以及`shader`中。

`light.h`以及`light.cpp`仅是分装了多光源的初始化（读取着色器代码创建shader程序，以及初始化对象），以及光源shader值的初始化与使用，点光源的绘制。光源真正的计算体现在shader中。

多光源系统的shader在shader文件夹下的`multi_light.fs`以及`multi_light.vs`中，其中重点为片元着色器。（工程中实际采用的是`1.color.frag`以及`1.color.vert`这二者中有阴影部分的实现，但是无法完整体现多光源的效果）

`multi_light.fs`中有如下三种结构体，分别定义了点光源、平行光，聚光的相关参数。

```c++
struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};
```

同时也定义了材质结构体，包含镜面反射贴图，漫反射贴图以及表示反光度的参数。

```c++
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
```

多光源效果，在于通过多种光照的计算后，将效果叠加体现，故以下三种函数分别计算平行光、点光源、聚光的光照效果，每次计算时将考虑材质中镜面反射、漫反射的影响（实际上也是通过两次计算完成）。

```c++
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
```

最终将三种计算效果在主函数中叠加来体现(多个点光源需要通过多次计算)

```c++
// 第一阶段：定向光照
vec3 result = CalcDirLight(dirLight, norm, viewDir);
// 第二阶段：点光源
for(int i = 0; i < NR_POINT_LIGHTS; i++)
    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
// 第三阶段：聚光
result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
```

在最终工程中使用的`1.color.frag`中，没有体现出定向光照与聚光的效果，故没有这两部分所对应的计算。



### 3.9 多物体管理系统

#### 3.9.1 基本思想

为了有效管理加入到场景内的多种物体，需要用一种简单的方式来管理场景内的多种物体。因此本项目通过一种**多叉树**来管理新增到场景内的所有物体（即协助实现建模模式）。

树的叶子节点，即加入场景中的每一个物体，通过树的方式将这些叶子节点联系在一起，从而能够将子物体组成更大的物体，我们只需要访问一个树的某个节点，就可以绘制该节点下所有的叶子节点，或是调整它们的位置。

通过“左儿子右兄弟”的方式，用二叉树来实现多叉树。实现的树结构只需要三种基本操作，**创建根节点**，**合并两棵树**，以及**访问一个节点下所有的叶子节点**。

#### 3.9.2 相关函数说明

`ObjTree* CreatLeafnode`用于创建一个叶子节点，在创建叶子节点时需要一个无参的函数作为该物体的绘制函数，以及`model`矩阵来初始化物体的位置。

`ObjTree* UnionTree`用于合并两棵树，返回新产生的父节点。

访问一个节点下的叶子节点没有单独实现，但是体现在`void DrawObjCollection`以及中`void UpdateModel`中，前者可以绘制一个节点下的所有叶子节点，后者可以通过一个节点，更新所有叶子节点的`model`值，即可以实现多个物体的相同几何变换（可惜的是这一功能没有完全实现在项目中）



### 3.10  移动平台虚拟现实漫游系统

#### 3.10.1 感应控制的摄像机视角旋转

glfm里面封装了android系统提供的重力计、加速度传感器、陀螺仪和Rotation Vector传感器（一个软件传感器，综合了陀螺仪和地磁场传感器的信息得到的从手机坐标系到世界坐标系的过渡矩阵）。

VR中视角随着人手机朝向变化的功能主要利用了Rotation Vector传感器。传感器的世界坐标系里，y轴永远指向北，z轴指向正上方，x轴指东。手机的坐标系里，手机屏幕为xy平面，较短的边对应x方向，屏幕所对方向为z轴正方向。返回的是如下的矩阵：

根据这个矩阵可以调整vr显示的摄像机方向，我用的是横屏显示，视口的xyz轴分别对应手机坐标系的-y,x,z轴。

#### 3.10..2 双目绘制

分成两个视口。绘制前两个视口要单独清除深度缓冲，但是颜色缓冲只用每帧清空。暂时没有考虑到瞳距和显示位置等因素，只是将屏幕等分为两份分别绘制。

#### 3.10..3 畸变矫正

预备使用的VR原型装置的透镜较为简陋，预计将会有很大的径向畸变。畸变的参数可以用以下矩阵近似表示：

要求该畸变的逆变换并作用于projection矩阵得到正确的图像。（这方面还没有实现，透过透镜看到的是畸变后的图像，但其实对立体效果影响不大）

#### 3.10.4 飞行漫游

VR模式下用户不太能够使用手指操控，于是采用了类似飞行的漫游模式。用户视角会自动向前。为了增加真实感用户的前进方向并不能突然变化，而是以一个固定的加速度向用户的摄像机朝向转向。相机位置的更新发生在每次渲染循环的结束。