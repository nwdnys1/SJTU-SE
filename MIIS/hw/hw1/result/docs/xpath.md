## 用 Xpath 定位元素

1. 打开示例网址 https://github.com/aisingapore/TagUI 并在页面中进行下面的搜索。

### 浏览器中复制 xpath

1. F12

2. Ctrl + F 后可以搜索，比如在搜索框中输入下方 xpath 之后、按下 enter、页面中会有很多个高亮的元素。

   ```xpath
   //*[text()="TagUI"]
   ```

3. 这是因为这种 xpath 中开头的 // 表示从任意位置开始查找，所以会找到很多个元素，如果输入下面的 xpath，就会固定住找到的元素，因为在前面加了筛选。

   ```xpath
   //*[@class="flex-auto min-width-0 width-fit"]//*[text()="TagUI"]
   ```

4. 右键找到的元素， Copy -> Copy xpath，即可复制 xpath。但是这样复制下来的 xpath 很长（像下面这样）。所以也可以只复制其中的类名，id 或者元素名，然后自己拼接 xpath，就像这些例子一样。方法：双击这个元素中的一部分、然后复制就行。

   ```xpath
   //*[@id="repo-content-pjax-container"]/div/div/div[2]/div[1]/react-partial/div/div/div[3]/div[2]/div/div[2]/article/p[11]
   ```

### 常见 xpath

1. 通过元素的文本内容定位元素。下面的例子即在所有页面的元素中先找到 html 的 article 元素，然后在 article 元素中的任意位置开始找到文本内容为"TagUI"的元素。这里的 \* 表示任意元素，所以可以是 div、p、span 等等。方括号里的表示元素的其他内容，这里是 text()，表示元素的文本内容。

   ```xpath
   //article//*[text()="TagUI"]
   ```

2. 通过元素的属性定位元素。下面的例子即先在当前页面中找到所有 class 属性为"flex-auto min-width-0 width-fit"的元素，然后在这些元素中找到 img 元素。注意后面的 div 和 img 前都是单斜杠 / 。单斜杠 / 的意义即从当前元素直接开始查找，而双斜杠 // 的意义即从 html 中的任意位置开始查找。如果一开始就用单斜杠、即从 html 的根元素开始查找。方括号里的@表示属性，这里是 class 属性，也可以是 @id、@name 等等属性。

   ```xpath
   //*[@class="flex-auto min-width-0 width-fit"]/div/img
   ```

3. 同样地，如果想找到 img 元素的父元素，可以在 img 后面加上 /..，即父元素的 xpath。这里找到的其实就是 div 的元素。

   ```xpath
   //*[@class="flex-auto min-width-0 width-fit"]/div/img/..
   ```

4. 通过文本中部分包含的内容定位元素。（对应于上面所有的 Text 都要完全匹配，这里只需要部分匹配）

   ```xpath
   //article//*[contains(text(), "create and edit")]
   ```

5. 如果有多个元素，可以通过下标来定位元素。下面的例子即找到所有 class 属性为"flex-auto min-width-0 width-fit"的元素中的第一个元素或者第二个元素。

   ```xpath
   (//*[@class="flex-auto min-width-0 width-fit"])[1]
   (//*[@class="flex-auto min-width-0 width-fit"])[2]
   ```

6. 更多例子，也可以问 gpt

   ```xpath
   //*[@class="Box-sc-g0xbh4-0 iWFGlI"]//input
   ```
