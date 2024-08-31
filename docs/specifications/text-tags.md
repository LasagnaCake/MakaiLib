# Text Tags

## Overview

- Tags can be nested in eachother.

## Syntax

`[`*`chr-seq`*`:`*`text`*`]`, where:

- *`chr-seq`*: A sequence of [tags](#Tags) (as detailed below) defining the style of the proceeding *`text`*.
- *`text`*: A string of text.

`[` and `]` are escaped as `[[` and `]]`, respectively.

## Valid Tags

Spaces are ignored.

`+` and `-` tags are $$1/4$$ of the size of the font size.

- `b` for **bold**
- `i` for *italics*
- `u` for <ins>underline</ins>
- `s` for ~~strikethrough~~
- `+` for <sup>superscript</sup>
- `-` for <sub>subscript</sub>
- `@[link]` for [links](https://www.youtube.com/watch?v=ihCc2MoLF9k)
- - *`text`* is optional if this is tag included
- `=` for inserting values
- - To be loaded from a JSON structure attached to the `Label`, with `text` being the key
- `#[style]` for (limited) text stylization

### On the `#[style]` tag

`style` parameters are of the form `key=value`, and are separated via semicolons.

#### Available parameters

- `color`: Takes in a hex color (ex: `#00f`, `#f045cf0f`, `#fc0d`) as value
- `font`: Takes in an unsigned integer as a value
- - To be used as an index into an array of fonts


## Examples

|Text Tag|Resulting Text|
|:-:|:-|
|`[biu:abcdefg]`|<ins>***abcdefg***</ins>|
|`[+#[color=#f00]:colored text]`|<span style="color:red"><sup>colored text</sup></span>|
|`[bi@[http://example.com]:]`|[***http://example.com***](http://example.com)|
|`[bi@[http://example.com]:click me!]`|[***click me!***](http://example.com)|
|`[:text]`|text|
|`[::text]`|:text|
|`[text]`|\[text\]|
|`[`*`any-chr-seq`*`:]`||
|`[:]`||