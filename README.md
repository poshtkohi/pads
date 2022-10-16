# Implementing Parallel and Distributed Systems (PADS)

This is a home page regarding the source codes for the book **Implementing Parallel and Distributed Systems** published by Taylor & Francis/CRC Press. → <https://www.routledge.com/p/book/9781032151229>

![This is the book cover](/assets/images/book-cover.jpg)

## 1. A Linux Image for VMware

The authors provide a Linux’s VMware image which accompanies the source codes for all the chapters of the book. Of course, the reader could compile and prepare all the necessary stuff as described in the book by themselves. However, the image includes all the pre-installed tools and libraries, as well as Code::Block IDE, that the readers need to easily build and develop applications for Parvicrursor Platform (Chapters 1 to 10) and MPI-based Supercomputing (Chapter 11). 

In order to take advantage of this environment, please take the following steps carefully.

**A)** Download and install VMware Workstation Player on your operating system of choice via [this link]( https://www.vmware.com/uk/products/workstation-player.html).

**B)** Download the image files in a ZIP file partitioned into four parts from Google Drive (approximately, 4GB per file for a total of 13.4GB). To be able to download these big chunks, please ensure that you have logged in to your Google account first (if unavailable, create one before proceeding). Then, you can download the parts of the zipped VMware image from the links below:

[Download Part 1](https://drive.google.com/uc?export=download&id=1_KQMMyZyT5Xv7Mt_4d41NFpPnUP0WsTH)

[Download Part 2](https://drive.google.com/uc?export=download&id=1-oH2y8EdBQ1noDJFMrt7Q9WCKFMbyboq)

[Download Part 3](https://drive.google.com/uc?export=download&id=1c0bHnb3OMJGC5uB3z0gPDihHAOIm7CJx)

[Download Part 4](https://drive.google.com/uc?export=download&id=1c02grYEFd0ICGy_7gcMXICCidcQVST4s)

After total downloads complete, please unzip the four parts through an appropriate ZIP software.

**C)** Open the image, unzipped in step C, through VMware Workstation Player. Please use the following credential to log in to the Linux image:

**Username**: root **Password**: test12345

**D)** Refer to your Linux image's desktop and open Code::Blocks. Subsequently, you should come along the picture below:

![This is an image for Parvicursor development environment](/assets/images/codeblocks.png)

As seen there are two workspaces in Code::Blocks that have all the code examples for this book. The source directory can be found in */root/devel/book-src* within the Linux image. To use the Parvicursor project, two libraries and two executables must be built before working on Parvicursor examples, including, *ParvicursorLib*, *AsyncSocketLib*, *xDFSServer_exe*, and *xDFSClient_test*. It is necessary to note that before running xThread examples, the *xDFSServer_exe* project must be run beforehand.

## 2. A Distribution Package for Microsoft Visual Studio on Windows Family of Operating Systems

We will shortly release a ZIP package that the readers can open it via Visual Studio for rapid Windows-based development. Additionally, all Visual Studio's project files are included with the book's source codes that one can make use of them for importing into the latest version of their Visual Studio installation (of course, Crypto++ library must be manually installed in advance).

## 3. Questions and Support

If the respected readers have any questions about the Linux image or Windows package, please get in touch with the authors via the email address below

![Author's email address](/assets/images/email-new.png)

The authors would be delighted to get back to you as quick as possible.

